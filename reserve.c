/* reserve.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <raylib.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "reserve.h"

static struct PileVtable reserveVtable = {
    &ReserveCanMoveTail,
    &ReserveCanAcceptCard,
    &ReserveCanAcceptTail,
    &PileInertPileTapped,
    &PileGenericTailTapped,
    &ReserveCollect,
    &ReserveComplete,
    &PileGenericUnsortedPairs,

    &PileReset,
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

int ReserveCollect(struct Pile *const self)
{
    return PileGenericCollect(self);
}

_Bool ReserveComplete(struct Pile *const self)
{
    return PileEmpty(self);
}
