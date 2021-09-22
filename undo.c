/* undo.c */

#include <stdio.h>

#include "baize.h"
#include "array.h"

/*
    A snapshot of the baize is an array (that mimics the baize piles array) of pointers to arrays of card pointers
    (same as the Pile->cards)
*/

struct Array* SnapshotNew(struct Baize *const self)
{
    struct Array *savedPiles = ArrayNew(ArrayLen(self->piles));
    size_t index;
    for ( struct Pile *p = (struct Pile*)ArrayFirst(self->piles, &index); p; p = (struct Pile*)ArrayNext(self->piles, &index) ) {
        struct Array *savedCards = ArrayClone(p->cards);
        ArrayPush(savedPiles, savedCards);
    }
    return savedPiles;
}

void SnapshotFree(struct Array* savedPiles) {
    size_t pindex;
    // *savedPiles is an Array of Pile*
    for ( struct Array *p = (struct Array*)ArrayFirst(savedPiles, &pindex); p; p = (struct Array*)ArrayNext(savedPiles, &pindex) ) {
        // *p is an Array of Card*
        ArrayFree(p);
    }
    ArrayFree(savedPiles);
}

struct Array* UndoStackNew(void) {
    return ArrayNew(32);
}

void UndoStackFree(struct Array *stack) {
    // for ( struct Array* item = (struct Array*)ArrayPop(stack); item; item = (struct Array*)ArrayPop(stack) ) {
    //     SnapshotFree(item);
    // }
    ArrayForeach(stack, (ArrayIterFunc)ArrayFree);
    ArrayFree(stack);
}

void BaizeUndoPush(struct Baize *const self) {
    struct Array* savedPiles = SnapshotNew(self);
    ArrayPush(self->undoStack, savedPiles);
    // mark movable
    // recalc percent complete
    // update UI (stock, waste, moves, percent)
}   

struct Array* BaizeUndoPop(struct Baize *const self) {
    if ( ArrayLen(self->undoStack) > 0 ) {
        struct Array* savedPiles = ArrayPop(self->undoStack);
        return savedPiles;
    }
    return NULL;
}

void PileUpdateFromCardArray(struct Pile *const self, struct Array *cards)
{
    ArrayReset(self->cards);
    size_t cindex;
    for ( struct Card *c = (struct Card*)ArrayFirst(cards, &cindex); c; c = (struct Card*)ArrayNext(cards, &cindex) ) {
        PilePushCard(self, c);
        if ( c->id.prone ) {
            CardFlipDown(c);
        } else {
            CardFlipUp(c);
        }
    }
    // TODO scrunch this pile
}

void BaizeUpdateFromSnapshot(struct Baize *const self, struct Array *savedPiles)
{
    if ( ArrayLen(self->piles) != ArrayLen(savedPiles) ) {
        fprintf(stderr, "Bad snapshot\n");
    } else {
        for ( size_t i=0; i<ArrayLen(self->piles); i++ ) {
            struct Array* pSrc = ArrayGet(savedPiles, i);
            struct Pile* pDst = ArrayGet(self->piles, i);
            PileUpdateFromCardArray(pDst, pSrc);
        }
    }
}

void BaizeSavePositionCommand(struct Baize *const self)
{
    // if ( BaizeComplete(self) ) {
    //     fprintf(stderr, "*** Cannot bookmark a completed game ***\n");
    //     return;
    // }
    self->savedPosition = ArrayLen(self->undoStack);
    fprintf(stderr, "*** Position bookmarked ***\n");
    fprintf(stderr, "undoStack %lu savedPosition %lu\n", ArrayLen(self->undoStack), self->savedPosition);
}

void BaizeLoadPositionCommand(struct Baize *const self)
{
    fprintf(stderr, "undoStack 1 %lu\n", ArrayLen(self->undoStack));

    if ( self->savedPosition == 0 || self->savedPosition > ArrayLen(self->undoStack) ) {
        fprintf(stderr, "*** No bookmark ***\n");
        return;
    }
    // if ( BaizeComplete(self) ) {
    //     fprintf(stderr, "*** Cannot undo a completed game ***\n");
    //     return;
    // }
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
        fprintf(stderr, "*** Nothing to undo ***\n");
        return;
    }

    // if ( BaizeComplete(self) ) {
    //     fprintf(stderr, "*** Cannot undo a completed game ***");
    //     return;
    // }

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