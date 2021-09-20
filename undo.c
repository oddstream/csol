/* undo.c */

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
    for ( struct Array* item = (struct Array*)ArrayPop(stack); item; item = (struct Array*)ArrayPop(stack) ) {
        SnapshotFree(item);
    }
    ArrayFree(stack);
}

void UndoPush(struct Baize *const self) {
    struct Array* savedPiles = SnapshotNew(self);
    ArrayPush(self->undoStack, savedPiles);
    // mark movable
    // recalc percent complete
    // update UI (stock, waste, moves, percent)
}   

struct Array* UndoPop(struct Baize *const self) {
    if ( ArrayLen(self->undoStack) > 0 ) {
        struct Array* savedPiles = ArrayPop(self->undoStack);
        return savedPiles;
    }
    return NULL;
}
