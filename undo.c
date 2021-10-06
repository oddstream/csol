/* undo.c */

#include <stdio.h>
#include <string.h>

#include "array.h"
#include "baize.h"
#include "util.h"

#include "ui.h"

struct SavedCard {
    unsigned int index:15;
    unsigned int prone:1;
};

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
        for ( struct Card *c = (struct Card*)ArrayFirst(cards, &index); c; c = (struct Card*)ArrayNext(cards, &index) ) {
            self->sav[self->len++] = (struct SavedCard){.index = c - cardLibrary, .prone = c->prone};
        }
    }
    return self;
}

static void SavedCardArrayPopAll(struct SavedCardArray *const self, struct Card *const cardLibrary, struct Pile *const pile)
{
    for ( size_t i=0; i<self->len; i++ ) {
        struct SavedCard sc = self->sav[i];
        struct Card* c = &cardLibrary[sc.index];
        PilePushCard(pile, c);
        if ( sc.prone ) {
            CardFlipDown(c);
        } else {
            CardFlipUp(c);
        }
    }
}

static void SavedCardArrayFree(struct SavedCardArray *const sca)
{
    if ( sca ) {
        free(sca);
    }
}

/*
    A snapshot of the baize is an array (that mimics the baize piles array) of pointers to arrays of SavedCard objects
*/

static struct Array* SnapshotNew(struct Baize *const self)
{
    struct Array *savedPiles = ArrayNew(ArrayLen(self->piles));
    size_t pindex;
    for ( struct Pile *p = (struct Pile*)ArrayFirst(self->piles, &pindex); p; p = (struct Pile*)ArrayNext(self->piles, &pindex) ) {
        struct SavedCardArray* sca = SavedCardArrayNew(self->cardLibrary, p->cards);
        ArrayPush(savedPiles, sca);
    }
    return savedPiles;
}

static void SnapshotFree(struct Array* savedPiles)
{
    size_t pindex;
    for ( struct SavedCardArray *p = (struct SavedCardArray*)ArrayFirst(savedPiles, &pindex); p; p = (struct SavedCardArray*)ArrayNext(savedPiles, &pindex) ) {
        SavedCardArrayFree(p);
    }
    ArrayFree(savedPiles);
}

struct Array* UndoStackNew(void)
{
    return ArrayNew(32);
}

void UndoStackFree(struct Array *stack)
{
    for ( struct Array* item = (struct Array*)ArrayPop(stack); item; item = (struct Array*)ArrayPop(stack) ) {
        SnapshotFree(item);
    }
    // this would free the piles, but not the cards, so would leak (thank you valgrind)
    // ArrayForeach(stack, (ArrayIterFunc)ArrayFree);
    ArrayFree(stack);
}

static int CalcPercentComplete(struct Baize *const self)
{
#if 1
    int sorted = 0, unsorted = 0;
    size_t index;
    for ( struct Pile *p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        p->vtable->CountSortedAndUnsorted(p, &sorted, &unsorted);
    }
    return (int)(UtilMapValue((float)sorted-(float)unsorted, -(float)self->cardsInLibrary, (float)self->cardsInLibrary, 0.0f, 100.0f));
#else
    (void)self;
    return 42;
#endif
}

void BaizeUndoPush(struct Baize *const self)
{
    struct Array* savedPiles = SnapshotNew(self);
    ArrayPush(self->undoStack, savedPiles);
    // mark movable
    // recalc percent complete
    char zLeft[64], zCenter[64], zRight[64];

    sprintf(zCenter, "MOVES: %lu", ArrayLen(self->undoStack) - 1);

    sprintf(zRight, "COMPLETE: %d%%", CalcPercentComplete(self));

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

struct Array* BaizeUndoPop(struct Baize *const self) {
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

void BaizeSavePositionCommand(struct Baize *const self)
{
    UiHideNavDrawer(self->ui);

    if ( BaizeComplete(self) ) {
        UiToast(self->ui, "Cannot bookmark a completed game");
        return;
    }
    self->savedPosition = ArrayLen(self->undoStack);
    UiToast(self->ui, "Position bookmarked");
    // fprintf(stderr, "*** Position bookmarked ***\n");
    fprintf(stderr, "undoStack %lu savedPosition %lu\n", ArrayLen(self->undoStack), self->savedPosition);
}

void BaizeLoadPositionCommand(struct Baize *const self)
{
    UiHideNavDrawer(self->ui);

    fprintf(stderr, "undoStack 1 %lu\n", ArrayLen(self->undoStack));

    if ( self->savedPosition == 0 || self->savedPosition > ArrayLen(self->undoStack) ) {
        UiToast(self->ui, "No bookmark");
        fprintf(stderr, "*** No bookmark ***\n");
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
    fprintf(stderr, "undoStack 2 %lu\n", ArrayLen(self->undoStack));
}

void BaizeRestartDealCommand(struct Baize *const self)
{
    UiHideNavDrawer(self->ui);

    struct Array *snapshot = NULL;
    while ( ArrayLen(self->undoStack) > 0 ) {
        if ( snapshot ) {
            SnapshotFree(snapshot);
        }
        snapshot = ArrayPop(self->undoStack);
    }
    if ( snapshot ) {
        BaizeUpdateFromSnapshot(self, snapshot);
        SnapshotFree(snapshot);
    }
    self->savedPosition = 0;
    BaizeUndoPush(self);    // replace current state
}

void BaizeUndoCommand(struct Baize *const self)
{
    if ( ArrayLen(self->undoStack) < 2 ) {
        UiToast(self->ui, "Nothing to undo");
        return;
    }

    if ( BaizeComplete(self) ) {
        UiToast(self->ui, "Cannot undo a completed game");
        return;
    }

    struct Array* snapshot = BaizeUndoPop(self);    // removes current state
    if ( NULL == snapshot ) {
        fprintf(stderr, "*** Error popping from undo stack (1) ***\n");
        return;
    }
    SnapshotFree(snapshot);  // discard this one, it's the same as the current baize

    snapshot = BaizeUndoPop(self);    // removes current state
    if ( NULL == snapshot ) {
        fprintf(stderr, "*** Error popping from undo stack (2) ***\n");
        return;
    }
    BaizeUpdateFromSnapshot(self, snapshot);
    SnapshotFree(snapshot);

    BaizeUndoPush(self);    // replace current state
}