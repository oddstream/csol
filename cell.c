/* cell.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "cell.h"

static struct PileVtable cellVtable = {
    &CellCanMoveTail,
    &PileInertCanMatchTail,
    &CellCanAcceptCard,
    &CellCanAcceptTail,
    &PileGenericTailTapped,
    &CellCollect,
    &CellComplete,
    &CellConformant,
    &PileInertSetRecycles,
    &CellCountSortedAndUnsorted,

    &PileUpdate,
    &PileDraw,
    &PileFree,
};

struct Cell* CellNew(struct Baize *const baize, Vector2 slot, enum FanType fan)
{
    struct Cell* self = calloc(1, sizeof(struct Cell));
    if ( self ) {
        PileCtor(baize, (struct Pile*)self, "Cell", slot, fan);
        self->super.vtable = &cellVtable;
    }
    return self;
}

_Bool CellCanMoveTail(struct Array *const tail)
{
    (void)tail;
    return 1;
}

_Bool CellCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    (void)c;
    if ( !PileEmpty(self) ) {
        BaizeSetError(baize, "(CSOL) Can only move a card to an empty Cell");
        return 0;
    }
    return 1;
}

_Bool CellCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    if ( ArrayLen(tail) != 1 ) {
        BaizeSetError(baize, "(CSOL) Can only move a single card to a Cell");
        return 0;
    }
    if ( !PileEmpty(self) ) {
        BaizeSetError(baize, "(CSOL) Can only move a card to an empty Cell");
        return 0;
    }
    return 1;
}

int CellCollect(struct Pile *const self)
{
    return PileGenericCollect(self);
}

_Bool CellComplete(struct Pile *const self)
{
    return PileEmpty(self);
}

_Bool CellConformant(struct Pile *const self)
{
    (void)self;
    return 1;
}

void CellCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
    (void)unsorted;
    if ( ArrayLen(self->cards) == 1 ) {
        *sorted += 1;   // one sorted card
    }
}