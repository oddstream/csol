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
    &CellCanAcceptCard,
    &CellCanAcceptTail,
    &CellCollect,
    &CellComplete,
    &CellConformant,
    &CellSetAccept,
    &CellSetRecycles,
    &CellCountSortedAndUnsorted,

    &PileUpdate,
    &PileDraw,
    &PileFree,
};

struct Cell* CellNew(Vector2 slot, enum FanType fan)
{
    struct Cell* self = calloc(1, sizeof(struct Cell));
    if ( self ) {
        PileCtor((struct Pile*)self, "Cell", slot, fan);
        self->super.vtable = &cellVtable;
    }
    return self;
}

bool CellCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    (void)c;
    if ( !PileEmpty(self) ) {
        BaizeSetError(baize, "Can only move a card to an empty Cell");
        return false;
    }
    return true;
}

bool CellCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    if ( ArrayLen(tail) != 1 ) {
        BaizeSetError(baize, "Can only move a single card to a Cell");
        return false;
    }
    if ( !PileEmpty(self) ) {
        BaizeSetError(baize, "Can only move a card to an empty Cell");
        return false;
    }
    return true;
}

int CellCollect(struct Pile *const self)
{
    return PileGenericCollect(self);
}

bool CellComplete(struct Pile *const self)
{
    return PileEmpty(self);
}

bool CellConformant(struct Pile *const self)
{
    (void)self;
    return true;
}

void CellSetAccept(struct Pile *const self, enum CardOrdinal ord)
{
    // we don't do that here
    (void)self;
    (void)ord;
}

void CellSetRecycles(struct Pile *const self, int r)
{
    // we don't do that here
    (void)self;
    (void)r;
}

void CellCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
    (void)unsorted;
    if ( ArrayLen(self->cards) == 1 ) {
        *sorted += 1;   // one sorted card
    }
}