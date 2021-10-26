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
    &ReserveTapped,
    &ReserveCollect,
    &ReserveComplete,
    &ReserveConformant,
    &ReserveAccept,
    &ReserveSetAccept,
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

bool ReserveCanMoveTail(struct Array *const tail)
{
    (void)tail;
    return true;
}

bool ReserveCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    (void)self;
    (void)c;
    BaizeSetError(baize, "(CSOL) Cannot move a card to a Reserve");
    return false;
}

bool ReserveCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    BaizeSetError(baize, "(CSOL) Cannot move a card to a Reserve");
    (void)self;
    (void)tail;
    return false;
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

bool ReserveComplete(struct Pile *const self)
{
    return PileEmpty(self);
}

bool ReserveConformant(struct Pile *const self)
{
    return PileEmpty(self);
}

enum CardOrdinal ReserveAccept(struct Pile *const self)
{
    (void)self;
    return 0;
}

void ReserveSetAccept(struct Pile *const self, enum CardOrdinal ord)
{
    // we don't do that here
    (void)self;
    (void)ord;
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
