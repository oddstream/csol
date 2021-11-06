/* foundation.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "foundation.h"
#include "constraint.h"
#include "util.h"

static struct PileVtable foundationVtable = {
    &FoundationCanMoveTail,
    &PileInertCanMatchTail,
    &FoundationCanAcceptCard,
    &FoundationCanAcceptTail,
    &PileInertTapped,
    &PileInertCollect,
    &FoundationComplete,
    &FoundationConformant,
    &PileInertSetRecycles,
    &FoundationCountSortedAndUnsorted,

    &PileUpdate,
    &PileDraw,
    &PileFree,
};

struct Foundation* FoundationNew(struct Baize *const baize, Vector2 slot, enum FanType fan)
{
    struct Foundation* self = calloc(1, sizeof(struct Foundation));
    if ( self ) {
        PileCtor(baize, (struct Pile*)self, "Foundation", slot, fan);
        self->super.vtable = &foundationVtable;
    }
    return self;
}

_Bool FoundationCanMoveTail(struct Array *const tail)
{
    struct Card *c = ArrayGet(tail, 0);
    struct Baize* baize = PileOwner(CardOwner(c));
    BaizeSetError(baize, "(CSOL) Cannot move cards from a Foundation");
    (void)tail;
    return 0;
}

_Bool FoundationCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    if ( ArrayLen(self->cards) == baize->numberOfCardsInLibrary / ArrayLen(baize->foundations) ) {
        BaizeSetError(baize, "(CSOL) The foundation is full");
        return 0;
    }

    struct Array1 tail = Array1New(c);
    return CanTailBeAppended(self, (struct Array*)&tail);
    // don't need to free an Array1
}

_Bool FoundationCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    if (ArrayLen(tail) > 1) {
        BaizeSetError(baize, "(CSOL) Can only move a single card to a Foundation");
        return 0;
    }
    if (ArrayLen(self->cards) == baize->numberOfCardsInLibrary / ArrayLen(baize->foundations)) {
        BaizeSetError(baize, "(CSOL) The Foundation is full");
        return 0;
    }
    if (ArrayLen(self->cards) + ArrayLen(tail) > baize->numberOfCardsInLibrary / ArrayLen(baize->foundations)) {
        BaizeSetError(baize, "(CSOL) That would over-fill the Foundation");
        return 0;
    }
    // ArrayLen(tail) == 1
    return FoundationCanAcceptCard(baize, self, ArrayGet(tail, 0));
}

_Bool FoundationComplete(struct Pile *const self)
{
    struct Baize *baize = self->owner;
    return PileLen(self) == baize->numberOfCardsInLibrary / ArrayLen(baize->foundations);
}

_Bool FoundationConformant(struct Pile *const self)
{
    // a Foundation is always assumed to be conformant, how else did it get built!?
    (void)self;
    return 1;
}

void FoundationCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
    (void)unsorted;
    *sorted += ArrayLen(self->cards);
}
