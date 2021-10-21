/* undo.c */

#include <stdio.h>
#include <string.h>

#include "array.h"
#include "baize.h"
#include "stock.h"
#include "util.h"

#include "ui.h"

// We need to save the Card's prone flag as well as a reference (index) to the card in the card library
struct SavedCard {
    unsigned int index:15;
    unsigned int prone:1;
};

// Array functions store arrays of void* pointers, but we need to store arrays of indexes into the card library,
// so we have to make a new struct to avoid upsetting the compiler
struct SavedCardArray {
    size_t len;
    struct SavedCard sav[];
};

static struct SavedCardArray* SavedCardArrayNew(struct Card* cardLibrary, struct Array *const cards)
{
    struct SavedCardArray* self = calloc(1, sizeof(struct SavedCardArray) + ArrayLen(cards) * sizeof(struct SavedCard));
    if ( self ) {
        self->len = 0;
        size_t index;
        for ( struct Card *c = ArrayFirst(cards, &index); c; c = ArrayNext(cards, &index) ) {
            self->sav[self->len++] = (struct SavedCard){.index = c - cardLibrary, .prone = c->prone};
        }
    }
    return self;
}

static void SavedCardArrayPopAll(struct SavedCardArray *const sca, struct Card *const cardLibrary, struct Pile *const pile)
{
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
    fprintf(f, "%lu:", sca->len);
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
    A snapshot of the baize is an array (that mimics the baize piles array) of pointers to arrays (of SavedCardArray) of SavedCard objects
*/

static struct Array* SnapshotNew(struct Baize *const self)
{
    struct Array *savedPiles = ArrayNew(ArrayLen(self->piles));
    size_t pindex;
    for ( struct Pile *p = (struct Pile*)ArrayFirst(self->piles, &pindex); p; p = (struct Pile*)ArrayNext(self->piles, &pindex) ) {
        struct SavedCardArray* sca = SavedCardArrayNew(self->cardLibrary, p->cards);
        savedPiles = ArrayPush(savedPiles, sca);
    }
    return savedPiles;
}

static void SnapshotFree(struct Array* savedPiles)
{
    size_t pindex;
    for ( struct SavedCardArray *sca = ArrayFirst(savedPiles, &pindex); sca; sca = ArrayNext(savedPiles, &pindex) ) {
        SavedCardArrayFree(sca);
    }
    ArrayFree(savedPiles);
}

static void SnapshotWriteToFile(FILE* f, struct Array* savedPiles)
{
    size_t pindex;
    for ( struct SavedCardArray *sca = ArrayFirst(savedPiles, &pindex); sca; sca = ArrayNext(savedPiles, &pindex) ) {
        SavedCardArrayWriteToFile(f, sca);
    }
}

struct Array* UndoStackNew(void)
{
    return ArrayNew(32);
}

void UndoStackFree(struct Array *stack)
{
    for ( struct Array* item = ArrayPop(stack); item; item = ArrayPop(stack) ) {
        SnapshotFree(item);
    }
    // this would free the piles, but not the cards, so would leak (thank you valgrind)
    // ArrayForeach(stack, (ArrayIterFunc)ArrayFree);
    ArrayFree(stack);
}

static int CalcPercentComplete(struct Baize *const self)
{
    int sorted = 0, unsorted = 0;
    size_t index;
    for ( struct Pile *p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        p->vtable->CountSortedAndUnsorted(p, &sorted, &unsorted);
    }
    return (int)(UtilMapValue((float)sorted-(float)unsorted, -(float)self->numberOfCardsInLibrary, (float)self->numberOfCardsInLibrary, 0.0f, 100.0f));
}

void BaizeUndoPush(struct Baize *const self)
{
    struct Array* savedPiles = SnapshotNew(self);
    self->undoStack = ArrayPush(self->undoStack, savedPiles);
    // mark movable
    // recalc percent complete
    char zLeft[64], zCenter[64], zRight[64];

    sprintf(zCenter, "MOVES: %lu", ArrayLen(self->undoStack) - 1);

    sprintf(zRight, "COMPLETE: %d%%", CalcPercentComplete(self));

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

struct Array* BaizeUndoPop(struct Baize *const self)
{
    if ( ArrayLen(self->undoStack) > 0 ) {
        struct Array* savedPiles = ArrayPop(self->undoStack);
        return savedPiles;
    }
    return NULL;
}

static void UpdateFromSavedCardArray(struct Baize *const baize, struct Pile *const pile, struct SavedCardArray *sca)
{
    ArrayReset(pile->cards);
    SavedCardArrayPopAll(sca, baize->cardLibrary, pile);
    // TODO scrunch this pile
}

void BaizeUpdateFromSnapshot(struct Baize *const self, struct Array *savedPiles)
{
    if ( ArrayLen(self->piles) != ArrayLen(savedPiles) ) {
        fprintf(stderr, "Bad snapshot\n");
        return;
    }
    for ( size_t i=0; i<ArrayLen(self->piles); i++ ) {
        struct SavedCardArray *sca = ArrayGet(savedPiles, i);
        struct Pile* pDst = ArrayGet(self->piles, i);
        UpdateFromSavedCardArray(self, pDst, sca);
    }
}

void BaizeSavePositionCommand(struct Baize *const self, void* param)
{
    (void)param;

    UiHideDrawers(self->ui); // TODO should this be here?

    if ( BaizeComplete(self) ) {
        UiToast(self->ui, "Cannot bookmark a completed game");
        return;
    }
    self->savedPosition = ArrayLen(self->undoStack);
    UiToast(self->ui, "Position bookmarked");
    // fprintf(stderr, "*** Position bookmarked ***\n");
    fprintf(stderr, "undoStack %lu savedPosition %lu\n", ArrayLen(self->undoStack), self->savedPosition);
}

void BaizeLoadPositionCommand(struct Baize *const self, void* param)
{
    (void)param;
    UiHideDrawers(self->ui); // TODO should this be here?

    // fprintf(stderr, "undoStack 1 %lu\n", ArrayLen(self->undoStack));

    if ( self->savedPosition == 0 || self->savedPosition > ArrayLen(self->undoStack) ) {
        UiToast(self->ui, "No bookmark");
        return;
    }
    if ( BaizeComplete(self) ) {
        UiToast(self->ui, "Cannot undo a completed game");
        return;
    }
    struct Array *snapshot = NULL;
    while ( ArrayLen(self->undoStack) + 1 > self->savedPosition ) {
        if ( snapshot ) {
            SnapshotFree(snapshot);
        }
        snapshot = ArrayPop(self->undoStack);
    }
    if ( snapshot ) {
        BaizeUpdateFromSnapshot(self, snapshot);
        SnapshotFree(snapshot);
    }
    BaizeUndoPush(self);    // replace current state
    // fprintf(stderr, "undoStack 2 %lu\n", ArrayLen(self->undoStack));
}

void BaizeRestartDealCommand(struct Baize *const self)
{
    UiHideDrawers(self->ui); // TODO should this be here?

    struct Array *snapshot = NULL;
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
    BaizeUndoPush(self);    // replace current state

    BaizeStartGame(self);
}

#if 0
void BaizeUndo(struct Baize *const self)
{
    struct Array *snapshot = BaizeUndoPop(self);    // removes current state
    if (!snapshot) {
        fprintf(stderr, "ERROR: %s: popping from undo stack\n", __func__);
        return;
    }
    BaizeUpdateFromSnapshot(self, snapshot);
    SnapshotFree(snapshot);

    BaizeUndoPush(self);    // replace current state
}
#endif

void BaizeUndoCommand(struct Baize *const self, void* param)
{
    (void)param;

    if ( ArrayLen(self->undoStack) < 2 ) {
        UiToast(self->ui, "Nothing to undo");
        return;
    }

    if ( BaizeComplete(self) ) {
        UiToast(self->ui, "Cannot undo a completed game");
        return;
    }

    struct Array* snapshot = BaizeUndoPop(self);    // removes current state
    if (!snapshot) {
        fprintf(stderr, "ERROR: %s: popping from undo stack (1)\n", __func__);
        return;
    }
    SnapshotFree(snapshot);  // discard this one, it's the same as the current baize

    snapshot = BaizeUndoPop(self);    // removes current state
    if (!snapshot) {
        fprintf(stderr, "ERROR: %s: popping from undo stack (2)\n", __func__);
        return;
    }
    BaizeUpdateFromSnapshot(self, snapshot);
    SnapshotFree(snapshot);

    BaizeUndoPush(self);    // replace current state
}

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static bool createDirectories(char *src)
{
    if (!src || *src != '/') {
        fprintf(stderr, "ERROR: %s: bad input\n", __func__);
        return false;
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
                return false;
            } else {
                fprintf(stderr, "INFO: %s: %s created\n", __func__, dirName);
            }
        } else {
            fprintf(stdout, "INFO: %s: %s exists\n", __func__, dirName);
        }
    }
    return true;
}

void BaizeSaveUndoToFile(struct Baize *const self)
{
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
            for ( struct Array *savedPiles = ArrayFirst(self->undoStack, &index); savedPiles; savedPiles = ArrayNext(self->undoStack, &index) ) {
                fprintf(f, "#%lu\n", index);
                SnapshotWriteToFile(f, savedPiles);
            }
        }
        fclose(f);
    } else {
        fprintf(stderr, "ERROR: %s: could not create %s\n", __func__, fname);
    }

    // fprintf(stdout, "GetWorkingDirectory is %s\n", GetWorkingDirectory());
}

struct Array* LoadUndoFromFile(char *variantName)
{
    char *homeDir;
    if ((homeDir = getenv("HOME")) == NULL) {
        fprintf(stderr, "ERROR: %s: HOME not set\n", __func__);
        return NULL;
    }

    fprintf(stdout, "INFO: %s: HOME is %s\n", __func__, homeDir);

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
            // prepend a space to '#' to consume \n
            if (fscanf(f, " #%lu", &ncheck) != 1) {
                fprintf(stderr, "ERROR: %s: no more saved piles\n", __func__);
                goto fclose_label;
            }
            if (n != ncheck) {
                fprintf(stderr, "ERROR: %s: saved pile miscount %lu != %lu\n", __func__, n, ncheck);
                goto fclose_label;
            }
            struct Array *savedPiles = ArrayNew(pileCount);
            if (!savedPiles) {
                goto fclose_label;
            }
            for ( size_t m=0; m<pileCount; m++ ) {
                size_t cards;
                // prepend a space to consume \n
                if (fscanf(f, " %lu:", &cards) != 1) {
                    fprintf(stderr, "ERROR: %s: expecting number of cards\n", __func__);
                    goto fclose_label;
                }
                struct SavedCardArray *sca = calloc(1, sizeof(struct SavedCardArray) + cards * sizeof(struct SavedCard));
                if (!sca) {
                    goto fclose_label;
                }
                for (size_t card=0; card<cards; card++) {
                    int index, prone;
                    unsigned int number;
                    // if (fscanf(f, " %d/%d", &index, &prone) != 2) {
                    if (fscanf(f, " %u", &number) != 1) {
                        fprintf(stderr, "ERROR: %s: expecting card index<<1|prone\n", __func__);
                        goto fclose_label;
                    }
                    index = number >> 1;
                    prone = number & 1;
                    sca->sav[sca->len++] = (struct SavedCard){.index=index, .prone=prone};
                }
                savedPiles = ArrayPush(savedPiles, sca);
            }
            undoStack = ArrayPush(undoStack, savedPiles);
        }
        fclose_label: fclose(f);
    }

    return undoStack;
}
