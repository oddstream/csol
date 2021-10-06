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
#include "check.h"
#include "util.h"

static struct PileVtable foundationVtable = {
    &FoundationCanAcceptCard,
    &FoundationCanAcceptTail,
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

struct Foundation* FoundationNew(Vector2 slot, enum FanType fan)
{
    struct Foundation* self = calloc(1, sizeof(struct Foundation));
    if ( self ) {
        PileCtor((struct Pile*)self, "Foundation", slot, fan);
        self->super.vtable = &foundationVtable;
        self->accept = 0; // accept any by default
    }
    return self;
}

bool FoundationCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    BaizeResetError(baize);

    if ( ArrayLen(self->cards) == 13 ) {
        BaizeSetError(baize, "The foundation is full");
        return false;
    }

    if ( PileEmpty(self) ) {
        return CheckAccept(baize, self, c);
    }
    return CheckPair(baize, PilePeekCard(self), c);
}

bool FoundationCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    BaizeResetError(baize);
    // not true for Spider!
    // if ( ArrayLen(tail) != 1 ) {
    //     strcpy(self->owner->errorString, "Can only move single cards to a foundation");
    //     return false;
    // }
    if ( ArrayLen(self->cards) == 13 ) {
        BaizeSetError(baize, "The foundation is full");
        return false;
    }
    if ( ArrayLen(self->cards) + ArrayLen(tail) > 13 ) {
        BaizeSetError(baize, "That would make the foundation over full");
        return false;
    }
    if ( ArrayLen(tail) > 1 ) {
        // TODO for Spider, invent a new pile category (Discard) than can accept a run of 13 cards
        BaizeSetError(baize, "Can only move a single card to a Foundation");
        return false;
    }
    if ( PileEmpty(self) ) {
        return CheckAccept(baize, self, ArrayGet(tail, 0));
    }
    return CheckPair(baize, PilePeekCard(self), ArrayGet(tail, 0));
}

int FoundationCollect(struct Pile *const self)
{
    (void)self;
    return 0;
}

bool FoundationComplete(struct Pile *const self)
{
    return PileLen(self) == 13;
}

bool FoundationConformant(struct Pile *const self)
{
    (void)self;
    return true;
}

void FoundationSetAccept(struct Pile *const self, enum CardOrdinal ord)
{
    ((struct Foundation*)self)->accept = ord;
}

void FoundationSetRecycles(struct Pile *const self, int r)
{
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
    extern Color baizeHighlightColor;
    extern Font fontAcme;

    PileDraw(self);

    struct Foundation* f = (struct Foundation*)self;
    if ( f->accept != 0 ) {
        extern float cardWidth;
        float fontSize = cardWidth / 2.0f;
        Vector2 pos = PileScreenPos(self);
        pos.x += cardWidth / 8.0f;
        pos.y += cardWidth / 16.0f;
        DrawTextEx(fontAcme, UtilOrdToShortString(f->accept), pos, fontSize, 0, baizeHighlightColor);
    }
}
