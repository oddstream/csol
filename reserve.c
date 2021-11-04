/* reserve.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "reserve.h"

static struct PileVtable reserveVtable = {
    &ReserveCanMoveTail,
    &ReserveCanAcceptCard,
    &ReserveCanAcceptTail,
    &PileGenericTapped,
    &ReserveCollect,
    &ReserveComplete,
    &ReserveConformant,
    &ReserveSetRecycles,
    &ReserveCountSortedAndUnsorted,

    &PileUpdate,
    &PileDraw,
    &PileFree,
};

struct Reserve* ReserveNew(struct Baize *const baize, Vector2 slot, enum FanType fan)
{
    struct Reserve* self = calloc(1, sizeof(struct Reserve));
    if ( self ) {
        PileCtor(baize, (struct Pile*)self, "Reserve", slot, fan);
        self->super.vtable = &reserveVtable;
    }
    return self;
}

_Bool ReserveCanMoveTail(struct Array *const tail)
{
    (void)tail;
    return 1;
}

_Bool ReserveCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    (void)self;
    (void)c;
    BaizeSetError(baize, "(CSOL) Cannot move a card to a Reserve");
    return 0;
}

_Bool ReserveCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    BaizeSetError(baize, "(CSOL) Cannot move a card to a Reserve");
    (void)self;
    (void)tail;
    return 0;
}

void ReserveTapped(struct Pile *const self, struct Array *const tail)
{
    (void)self;
    (void)tail;
}

int ReserveCollect(struct Pile *const self)
{
    return PileGenericCollect(self);
}

_Bool ReserveComplete(struct Pile *const self)
{
    return PileEmpty(self);
}

_Bool ReserveConformant(struct Pile *const self)
{
    return PileEmpty(self);
}

void ReserveSetRecycles(struct Pile *const self, int r)
{
    // we don't do that here
    (void)self;
    (void)r;
}

void ReserveCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
    (void)sorted;
    *unsorted += ArrayLen(self->cards);
}
