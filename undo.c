/* undo.c */

#include <stdio.h>
#include <string.h>

#include "array.h"
#include "baize.h"
#include "stock.h"
#include "undo.h"
#include "util.h"

#include "ui.h"

/*
    BaizeResetState() creates a new, empty undo stack, then does an undo push, to save the starting state.

    The number of user moves is the length of the undo stack minus one.

    After a user move, there is a new undo push, to push the current state on the undo stack.

    So the top of the undo stack is always the "pre move" state shown on the baize.

    An undo command means popping the state from the top of the stack, and throwing it away.
    A second state is popped, and the baize restored from that. Finally, the current state is pushed back onto the stack.

    Restarting a deal works by popping all the items off the undo stack, restoring the baize from the last-popped state,
    then pushing a new state onto the stack.

    Bookmarking a position means remembering the current length of the undo stack.

    Going back to a bookmark makes means popping saved states off the undo stack until it's length == the saved length, 
    restoring the baize from the last-popped state, and then pushing a new state onto the stack.
*/

#define EMPTY_LABEL ("*")

static void encodeLabel(char *dst, char *src)
{
    if (*src == '\0') {
        strcpy(dst, EMPTY_LABEL);
        return;
    }
    while (*src) {
        if (*src == ' ' || *src == '\t')
            *dst++ = '_';
        else
            *dst++ = *src;
        src++;
    }
    *dst = '\0';
}

static void decodeLabel(char *dst, char *src)
{
    if (strcmp(src, EMPTY_LABEL) == 0) {
        *dst = '\0';
        return;
    }
    while (*src) {
        if (*src == '_')
            *dst++ = ' ';
        else
            *dst++ = *src;
        src++;
    }
    *dst = '\0';
}

// We need to save the Card's prone flag as well as a reference (index) to the card in the card library
struct SavedCard {
    unsigned int index:15;
    unsigned int prone:1;
};

// Array functions store arrays of void* pointers, but we need to store arrays of indexes into the card library,
// so we have to make a new struct to avoid upsetting the compiler
struct SavedCardArray {
    char label[MAX_PILE_LABEL+1];
    size_t len;
    struct SavedCard sav[];
};

static struct SavedCardArray* SavedCardArrayNew(struct Card* cardLibrary, struct Pile *const pile)
{
    struct SavedCardArray* self = calloc(1, sizeof(struct SavedCardArray) + ArrayLen(pile->cards) * sizeof(struct SavedCard));
    if (self) {
        encodeLabel(self->label, pile->label);
        self->len = 0;
        size_t index;
        for ( struct Card *c = ArrayFirst(pile->cards, &index); c; c = ArrayNext(pile->cards, &index) ) {
            self->sav[self->len++] = (struct SavedCard){.index = c - cardLibrary, .prone = CardProne(c)};
        }
    }
    return self;
}

static void SavedCardArrayCopyToPile(struct SavedCardArray *const sca, struct Card *const cardLibrary, struct Pile *const pile)
{
    char label[MAX_PILE_LABEL+1];
    decodeLabel(label, sca->label);
    pile->vtable->SetLabel(pile, label);

    for ( size_t i=0; i<sca->len; i++ ) {
        struct SavedCard sc = sca->sav[i];
        struct Card* c = &cardLibrary[sc.index];
        PilePushCard(pile, c);
        if ( sc.prone ) {
            CardFlipDown(c);
        } else {
            CardFlipUp(c);
        }
    }
}

static void SavedCardArrayWriteToFile(FILE* f, struct SavedCardArray *const sca)
{
    // sca->label will not be empty
    if (sca->label[0]=='\0') fprintf(stderr, "ERROR: %s: label is empty\n", __func__);

    fprintf(f, "%s %lu:", sca->label, sca->len);
    for ( size_t i=0; i<sca->len; i++ ) {
        unsigned int number = sca->sav[i].index;
        number <<= 1;
        number |= sca->sav[i].prone;
        fprintf(f, " %u", number);
        // fprintf(f, " %d/%d", sca->sav[i].index, sca->sav[i].prone);
    }
    fprintf(f, "\n");
}

static void SavedCardArrayFree(struct SavedCardArray *const sca)
{
    if (sca) {
        free(sca);
    }
}

/*
    A snapshot of the baize is an object that contains
        (1) the stock recycles count and
        (b) an array (that mimics the baize piles array) of pointers to arrays (of SavedCardArray) of SavedCard objects
*/
static struct Snapshot* SnapshotNew(struct Baize *const self)
{
    struct Snapshot *s = calloc(1, sizeof(struct Snapshot));
    if (s) {
        s->recycles = ((struct Stock*)self->stock)->recycles;
        s->savedPiles = ArrayNew(ArrayLen(self->piles));
        if (s->savedPiles) {
            size_t pindex;
            for ( struct Pile *p = (struct Pile*)ArrayFirst(self->piles, &pindex); p; p = (struct Pile*)ArrayNext(self->piles, &pindex) ) {
                struct SavedCardArray* sca = SavedCardArrayNew(self->cardLibrary, p);
                s->savedPiles = ArrayPush(s->savedPiles, sca);
            }
        }
    }
    return s;
}

static void SnapshotFree(struct Snapshot *s)
{
    size_t pindex;
    for ( struct SavedCardArray *sca = ArrayFirst(s->savedPiles, &pindex); sca; sca = ArrayNext(s->savedPiles, &pindex) ) {
        SavedCardArrayFree(sca);
    }
    ArrayFree(s->savedPiles);
    free(s);
}

static void SnapshotWriteToFile(FILE* f, size_t index, struct Snapshot *s)
{
    fprintf(f, "#%lu %d\n", index, s->recycles);

    size_t pindex;
    for ( struct SavedCardArray *sca = ArrayFirst(s->savedPiles, &pindex); sca; sca = ArrayNext(s->savedPiles, &pindex) ) {
        SavedCardArrayWriteToFile(f, sca);
    }
}

struct Array* UndoStackNew(void)
{
    return ArrayNew(32);
}

void UndoStackFree(struct Array *stack)
{
    ArrayForeach(stack, (ArrayIterFunc)SnapshotFree);
    ArrayFree(stack);
}

void BaizeUndoPush(struct Baize *const self)
{
    struct Snapshot* s = SnapshotNew(self);
    self->undoStack = ArrayPush(self->undoStack, s);

    // TODO mark movable?

    char zLeft[64], zCenter[64], zRight[64];

    sprintf(zCenter, "MOVES: %lu", ArrayLen(self->undoStack) - 1);

    sprintf(zRight, "COMPLETE: %d%%", self->exiface->PercentComplete(self));

    if (self->stock) {
        // Baize->stock may not be set yet if variant.lua/Build has not been run
        if ( PileHidden(self->stock) ) {
            UiUpdateStatusBar(self->ui, NULL, zCenter, zRight);
        } else {
            if ( self->waste ) {
                sprintf(zLeft, "STOCK: %lu WASTE: %lu", PileLen(self->stock), PileLen(self->waste));
            } else {
                sprintf(zLeft, "STOCK: %lu", PileLen(self->stock));
            }
            UiUpdateStatusBar(self->ui, zLeft, zCenter, zRight);
        }
    }
}

void BaizeUpdateFromSnapshot(struct Baize *const self, struct Snapshot *snap)
{
    if ( ArrayLen(self->piles) != ArrayLen(snap->savedPiles) ) {
        fprintf(stderr, "ERROR: %s: Bad snapshot\n", __func__);
        return;
    }
    for ( size_t i=0; i<ArrayLen(self->piles); i++ ) {
        struct SavedCardArray *sca = ArrayGet(snap->savedPiles, i);
        struct Pile* dstPile = ArrayGet(self->piles, i);
        ArrayReset(dstPile->cards);

        SavedCardArrayCopyToPile(sca, self->cardLibrary, dstPile);

        if (snap->recycles != ((struct Stock*)self->stock)->recycles) {
            self->stock->vtable->SetRecycles(self->stock, snap->recycles);
            lua_pushinteger(self->L, snap->recycles);
            lua_setglobal(self->L, "STOCK_RECYCLES");
        }
        // TODO scrunch this pile
    }
}

void BaizeSavePositionCommand(struct Baize *const self, void* param)
{
    (void)param;

    if ( BaizeComplete(self) ) {
        UiToast(self->ui, "Cannot bookmark a completed game");
        return;
    }
    self->savedPosition = ArrayLen(self->undoStack);
    UiToast(self->ui, "Position bookmarked");
    // fprintf(stdout, "undoStack %lu savedPosition %lu\n", ArrayLen(self->undoStack), self->savedPosition);
}

void BaizeLoadPositionCommand(struct Baize *const self, void* param)
{
    (void)param;

    // fprintf(stderr, "undoStack 1 %lu\n", ArrayLen(self->undoStack));

    if ( self->savedPosition == 0 || self->savedPosition > ArrayLen(self->undoStack) ) {
        UiToast(self->ui, "No bookmark");
        return;
    }
    // WHY can't you undo a completed game? Because undoing and then making a move counts as another win?
    // if ( BaizeComplete(self) ) {
    //     UiToast(self->ui, "Cannot undo a completed game");
    //     return;
    // }
    struct Snapshot *snapshot = NULL;
    while ( ArrayLen(self->undoStack) + 1 > self->savedPosition ) {
        if ( snapshot ) {
            SnapshotFree(snapshot);
        }
        snapshot = ArrayPop(self->undoStack);
    }
    if (snapshot) {
        BaizeUpdateFromSnapshot(self, snapshot);
        SnapshotFree(snapshot);
    }
    BaizeUndoPush(self);    // replace current state
    // fprintf(stderr, "undoStack 2 %lu\n", ArrayLen(self->undoStack));
}

void BaizeRestartDealCommand(struct Baize *const self, void* param)
{
    (void)param;

    struct Snapshot *snapshot = NULL;
    while ( ArrayLen(self->undoStack) > 0 ) {
        if (snapshot) {
            SnapshotFree(snapshot);
        }
        snapshot = ArrayPop(self->undoStack);
    }
    if (snapshot) {
        BaizeUpdateFromSnapshot(self, snapshot);
        SnapshotFree(snapshot);
    }
    self->savedPosition = 0;
    // do not run BaizeStartGame(self);!
    BaizeUndoPush(self);
}

void BaizeUndo0(struct Baize *const self)
{
    struct Snapshot *snap = ArrayPop(self->undoStack);    // removes current state
    if (!snap) {
        fprintf(stderr, "ERROR: %s: popping from undo stack of length %lu\n", __func__, ArrayLen(self->undoStack));
        return;
    }
    BaizeUpdateFromSnapshot(self, snap);
    SnapshotFree(snap);
}

void BaizeUndo(struct Baize *const self)
{
    if ( ArrayLen(self->undoStack) < 2 ) {
        fprintf(stderr, "WARNING: %s: Nothing to undo\n", __func__);
        return;
    }

    struct Snapshot* snap = ArrayPop(self->undoStack);    // removes current state
    if (!snap) {
        fprintf(stderr, "ERROR: %s: popping from undo stack\n", __func__);
        return;
    }
    SnapshotFree(snap);  // discard this one, it's the same as the current baize

    snap = ArrayPeek(self->undoStack);
    if (!snap) {
        fprintf(stderr, "ERROR: %s: peeking from undo stack\n", __func__);
        return;
    }
    BaizeUpdateFromSnapshot(self, snap);
}

void BaizeUndoCommand(struct Baize *const self, void* param)
{
    (void)param;

    if ( ArrayLen(self->undoStack) < 2 ) {
        UiToast(self->ui, "Nothing to undo");
        return;
    }

    // WHY can't you undo a completed game? Because undoing and then making a move counts as another win?
    // if ( BaizeComplete(self) ) {
    //     UiToast(self->ui, "Cannot undo a completed game");
    //     return;
    // }

    BaizeUndo(self);
}

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static _Bool createDirectories(char *src)
{
    if (!src || *src != '/') {
        fprintf(stderr, "ERROR: %s: bad input\n", __func__);
        return 0;
    }
    struct stat st = {0};
    char dirName[256];
    char *dst = dirName;
    while (*src) {
        do {
            // copy the first / and all chars up to but not including the next / or end of string
            *dst++ = *src++;
        } while (*src && *src != '/');
        *dst = '\0';

        if (stat(dirName, &st) == -1) {
            fprintf(stderr, "INFO: %s: %s does not exist\n", __func__, dirName);
            if (mkdir(dirName, 0700) == -1) {
                fprintf(stderr, "ERROR: %s: cannot create %s\n", __func__, dirName);
                return 0;
            } else {
                fprintf(stderr, "INFO: %s: %s created\n", __func__, dirName);
            }
        } else {
            // fprintf(stdout, "INFO: %s: %s exists\n", __func__, dirName);
        }
    }
    return 1;
}

void BaizeSaveUndoToFile(struct Baize *const self)
{
    extern int flag_nosave; if (flag_nosave) return;

    char *homeDir;
    if ((homeDir = getenv("HOME")) == NULL) {
        fprintf(stderr, "ERROR: %s: HOME not set\n", __func__);
        return;
    }

/*
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
*/

    fprintf(stdout, "INFO: %s: HOME is %s\n", __func__, homeDir);

    char fname[256];
    strcpy(fname, homeDir);
    strcat(fname, "/.config/oddstream.games/csol");
    if (!createDirectories(fname)) {
        return;
    }

    strcat(fname, "/saved.txt");
    FILE* f = fopen(fname, "w");
    if (f) {
        fprintf(f, "VARIANT=%s\n", self->variantName);
        fprintf(f, "PILES=%lu\n", ArrayLen(self->piles));
        fprintf(f, "UNDOSTACK=%lu\n", ArrayLen(self->undoStack));
        {
            size_t index;
            for ( struct Snapshot *s = ArrayFirst(self->undoStack, &index); s; s = ArrayNext(self->undoStack, &index) ) {
                SnapshotWriteToFile(f, index, s);
            }
        }
        fclose(f);
    } else {
        fprintf(stderr, "WARNING: %s: could not create %s\n", __func__, fname);
    }

    // fprintf(stdout, "GetWorkingDirectory is %s\n", GetWorkingDirectory());
}

struct Array* LoadUndoFromFile(char *variantName /* out */)
{
    extern int flag_noload; if (flag_noload) return NULL;

    char *homeDir;
    if ((homeDir = getenv("HOME")) == NULL) {
        fprintf(stderr, "ERROR: %s: HOME not set\n", __func__);
        return NULL;
    }

    // fprintf(stdout, "INFO: %s: HOME is %s\n", __func__, homeDir);

    char fname[256];
    strcpy(fname, homeDir);
    strcat(fname, "/.config/oddstream.games/csol/saved.txt");

    struct Array *undoStack = NULL;
    FILE* f = fopen(fname, "r");
    if (f) {
        size_t pileCount, stackDepth;

        /*
            "This implies that scanf will read across line boundaries to find its input, since newlines are white space.
            (White space characters are blank, tab, newline, carriage return, vertical tab, and formfeed.)" -- K&R p157
        */

        if (fscanf(f, "VARIANT=%[^\n]s", variantName) != 1) {
            fprintf(stderr, "ERROR: %s: cannot read VARIANT=\n", __func__);
            return NULL;
        }
// fprintf(stdout, "VARIANT='%s'\n", variantName);
        // prepend a space to 'PILES=' to consume \n
        if (fscanf(f, " PILES=%lu", &pileCount) != 1) {
            fprintf(stderr, "ERROR: %s: cannot read PILES=\n", __func__);
            return NULL;
        }
// fprintf(stdout, "PILES='%lu'\n", pileCount);
        // prepend a space to 'UNDOSTACK=' to consume \n
        if (fscanf(f, " UNDOSTACK=%lu", &stackDepth) != 1) {
            fprintf(stderr, "ERROR: %s: cannot read UNDOSTACK=\n", __func__);
            return NULL;
        }
// fprintf(stdout, "UNDOSTACK='%lu'\n", stackDepth);

        undoStack = ArrayNew(stackDepth + 16);
        for ( size_t n=0; n<stackDepth; n++) {
            size_t ncheck = 0xdeadbeef;
            int recycles;
            // prepend a space to '#' to consume \n
            if (fscanf(f, " #%lu %d", &ncheck, &recycles) != 2) {
                fprintf(stderr, "ERROR: %s: incorrect read of saved pile, expecting #number recycles\n", __func__);
                goto fclose_label;
            }
            if (n != ncheck) {
                fprintf(stderr, "ERROR: %s: saved pile miscount %lu != %lu\n", __func__, n, ncheck);
                goto fclose_label;
            }
            struct Snapshot *snap = calloc(1, sizeof(struct Snapshot));
            if (!snap) {
                goto fclose_label;
            }
            snap->recycles = recycles;
            snap->savedPiles = ArrayNew(pileCount);
            if (!snap->savedPiles) {
                free(snap);
                goto fclose_label;
            }
            for ( size_t m=0; m<pileCount; m++ ) {
                /*
                    <label> <count>: <cards>
                */
                char label[256];
                size_t cards;

                if (fscanf(f, "%s %lu:", label, &cards) != 2) {
                    fprintf(stderr, "ERROR: %s: expecting <label> <cards>:\n", __func__);
                    goto fclose_label;
                }

                struct SavedCardArray *sca = calloc(1, sizeof(struct SavedCardArray) + cards * sizeof(struct SavedCard));
                if (!sca) {
                    goto fclose_label;
                }
                if (strlen(label)>MAX_PILE_LABEL) {
                    sca->label[0] = '\0';
                } else {
                    strcpy(sca->label, label);
                }
                for (size_t card=0; card<cards; card++) {
                    int index, prone;
                    unsigned int number;
                    if (fscanf(f, " %u", &number) != 1) {
                        fprintf(stderr, "ERROR: %s: expecting card index<<1|prone\n", __func__);
                        goto fclose_label;
                    }
                    index = number >> 1;
                    prone = number & 1;
                    sca->sav[sca->len++] = (struct SavedCard){.index=index, .prone=prone};
                }
                snap->savedPiles = ArrayPush(snap->savedPiles, sca);
            }
            undoStack = ArrayPush(undoStack, snap);
        }
        fclose_label: fclose(f);

        remove(fname);
    }

#ifdef _DEBUG
    fprintf(stdout, "INFO: %s: state loaded from %s for '%s'\n", __func__, fname, variantName);
    if (undoStack)
        fprintf(stdout, "INFO: %s: returning undo stack of length %lu\n", __func__, ArrayLen(undoStack));
    else
        fprintf(stdout, "INFO: %s: returning null undo stack\n", __func__);

#endif

    return undoStack;
}
