/* discard.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "discard.h"
#include "constraint.h"
#include "util.h"

static struct PileVtable discardVtable = {
    &DiscardCanMoveTail,
    &PileInertCanAcceptCard,
    &DiscardCanAcceptTail,
    &PileInertPileTapped,
    &PileInertTailTapped,
    &PileInertCollect,
    &DiscardComplete,
    &DiscardConformant,
    &PileInertSetLabel,
    &PileInertSetRecycles,
    &DiscardCountSortedAndUnsorted,

    &PileUpdate,
    &DiscardDraw,
    &PileFree,
};

struct Discard* DiscardNew(struct Baize *const baize, Vector2 slot, enum FanType fan)
{
    struct Discard* self = calloc(1, sizeof(struct Discard));
    if ( self ) {
        PileCtor(baize, (struct Pile*)self, "Discard", slot, fan);
        self->super.vtable = &discardVtable;
    }
    return self;
}

_Bool DiscardCanMoveTail(struct Array *const tail)
{
    struct Card *c = ArrayGet(tail, 0);
    struct Baize* baize = PileOwner(CardOwner(c));
    BaizeSetError(baize, "(CSOL) Cannot move cards from a Discard");
    return 0;
}

_Bool DiscardCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    if ( !PileEmpty(self) ) {
        BaizeSetError(baize, "(CSOL) Can only move cards to an empty Discard");
        return 0;
    }
    int ndiscards = BaizeCountPiles(baize, self->category);
    if (ndiscards) {
        if ( ArrayLen(tail) != baize->numberOfCardsInLibrary / ndiscards ) {
            BaizeSetError(baize, "(CSOL) Can only move a full set of cards to a Discard");
            return 0;
        }
    }
    return CanTailBeAppended(self, tail);
}

_Bool DiscardComplete(struct Pile *const self)
{
    struct Baize *baize = self->owner;
    int ndiscards = BaizeCountPiles(baize, self->category);
    if (ndiscards) {
        return PileLen(self) == baize->numberOfCardsInLibrary / ndiscards;
    }
    return 0;
}

_Bool DiscardConformant(struct Pile *const self)
{
    struct Baize *baize = PileOwner(self);
    int ndiscards = BaizeCountPiles(baize, self->category);
    if (ndiscards) {
        return PileEmpty(self) || PileLen(self) == baize->numberOfCardsInLibrary / ndiscards;
    }
    return 0;
}

void DiscardCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
    (void)unsorted;
    *sorted += ArrayLen(self->cards);
}

void DiscardDraw(struct Pile *const self)
{
    extern Color baizeHighlightColor;

    struct Baize *baize = PileOwner(self);
    struct Pack *pack = baize->pack;

    Rectangle r = PileScreenRect(self);
    DrawRectangleRounded(r, pack->roundness, 9, baizeHighlightColor);
}
