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
#include "conformant.h"
#include "util.h"

static struct PileVtable foundationVtable = {
    &FoundationCardTapped,
    &FoundationPileTapped,
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

struct Foundation* FoundationNew(Vector2 slot, enum FanType fan, const char* buildfunc, const char* dragfunc)
{
    struct Foundation* self = calloc(1, sizeof(struct Foundation));
    if ( self ) {
        PileCtor((struct Pile*)self, "Foundation", slot, fan, buildfunc, dragfunc);
        self->super.vtable = &foundationVtable;
        self->accept = 0; // accept any by default
    }
    return self;
}

bool FoundationCardTapped(struct Card *c)
{   // not used
    BaizeResetError(c->owner->owner);
    return false;
}

bool FoundationPileTapped(struct Pile *p)
{
    BaizeResetError(p->owner);
    return false;
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
    if ( ArrayLen(self->cards) == 0 ) {
        struct Foundation *f = (struct Foundation*)self;
        if ( f->accept != 0 ) {
            struct Card* c = ArrayGet(tail, 0);
            if ( c->id.ordinal != f->accept ) {
                char z[128];
                sprintf(z, "This foundation can only accept a %d, not a %d", f->accept, c->id.ordinal);
                BaizeSetError(baize, z);
                return false;
            }
        }
        return ConformantBuild(baize, self, tail);
    }
    return ConformantBuild(baize, self, tail);
}

int FoundationCollect(struct Pile *const self)
{
    (void)self;
    return 0;
}

bool FoundationComplete(struct Pile *const self)
{
    (void)self;
    return false;
}

bool FoundationConformant(struct Pile *const self)
{
    (void)self;
    return false;
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
    extern Font fontAcme24;

    PileDraw(self);

    struct Foundation* f = (struct Foundation*)self;
    if ( f->accept != 0 ) {
        extern float cardWidth;
        float fontSize = cardWidth / 2.0f;
        Vector2 pos = PileScreenPos(self);
        pos.x += cardWidth / 8.0f;
        pos.y += cardWidth / 16.0f;
        DrawTextEx(fontAcme24, UtilOrdToShortString(f->accept), pos, fontSize, 0, baizeHighlightColor);
    }
}
