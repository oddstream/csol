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
    &FoundationCanAcceptCard,
    &FoundationCanAcceptTail,
    &FoundationTapped,
    &FoundationCollect,
    &FoundationComplete,
    &FoundationConformant,
    &FoundationSetAccept,
    &FoundationSetRecycles,
    &FoundationCountSortedAndUnsorted,

    &PileUpdate,
    &FoundationDraw,
    &PileFree,
};

struct Foundation* FoundationNew(struct Baize *const baize, Vector2 slot, enum FanType fan)
{
    struct Foundation* self = calloc(1, sizeof(struct Foundation));
    if ( self ) {
        PileCtor(baize, (struct Pile*)self, "Foundation", slot, fan);
        self->super.vtable = &foundationVtable;
        self->accept = 0; // accept any by default
    }
    return self;
}

bool FoundationCanMoveTail(struct Array *const tail)
{
    struct Card *c = ArrayGet(tail, 0);
    struct Baize* baize = CardToBaize(c);
    BaizeSetError(baize, "(CSOL) Cannot move cards from a Foundation");
    (void)tail;
    return false;
}

bool FoundationCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    if ( ArrayLen(self->cards) == baize->numberOfCardsInSuit ) {
        BaizeSetError(baize, "(CSOL) The foundation is full");
        return false;
    }

    struct Array1 tail =(struct Array1){.size=1, .used=1, .data[0]=c};
    return CanTailBeAppended(self, (struct Array*)&tail);
    // don't need to free an Array1
}

bool FoundationCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    if (ArrayLen(tail) > 1) {
        BaizeSetError(baize, "(CSOL) Can only move a single card to a Foundation");
        return false;
    }
    if (ArrayLen(self->cards) == baize->numberOfCardsInSuit) {
        BaizeSetError(baize, "(CSOL) The Foundation is full");
        return false;
    }
    if (ArrayLen(self->cards) + ArrayLen(tail) > baize->numberOfCardsInSuit) {
        BaizeSetError(baize, "(CSOL) That would over-fill the Foundation");
        return false;
    }
    // ArrayLen(tail) == 1
    return FoundationCanAcceptCard(baize, self, ArrayGet(tail, 0));
}

bool FoundationTapped(struct Pile *const self, struct Array *const tail)
{
    (void)self;
    (void)tail;
    return false;
}

int FoundationCollect(struct Pile *const self)
{
    // collecting is done by pulling cards to Foundations, so we do nothing here
    (void)self;
    return 0;
}

bool FoundationComplete(struct Pile *const self)
{
    return PileLen(self) == self->owner->numberOfCardsInSuit;
}

bool FoundationConformant(struct Pile *const self)
{
    // a Foundation is always assumed to be conformant, how else did it get built!?
    (void)self;
    return true;
}

void FoundationSetAccept(struct Pile *const self, enum CardOrdinal ord)
{
    ((struct Foundation*)self)->accept = ord;
}

void FoundationSetRecycles(struct Pile *const self, int r)
{
    // only the Stock can be recycled
    (void)self;
    (void)r;
}

void FoundationCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
    (void)unsorted;
    *sorted += ArrayLen(self->cards);
}

void FoundationDraw(struct Pile *const self)
{
    PileDraw(self);

    struct Foundation* f = (struct Foundation*)self;
    if ( f->accept != 0 ) {
        extern float cardWidth;
        extern Color baizeHighlightColor;
        extern Font fontAcme;
        float fontSize = cardWidth / 2.0f;
        Vector2 pos = PileScreenPos(self);
        pos.x += cardWidth / 8.0f;
        pos.y += cardWidth / 16.0f;
        DrawTextEx(fontAcme, UtilOrdToShortString(f->accept), pos, fontSize, 0, baizeHighlightColor);
    }
}
