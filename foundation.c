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
    &FoundationSetAccept,
    &FoundationSetRecycles,

    &PileUpdate,
    &FoundationDraw,
    &PileFree,
};

struct Foundation* FoundationNew(Vector2 slot, enum FanType fan, enum DragType drag, const char* buildfunc, const char* dragfunc)
{
    struct Foundation* self = malloc(sizeof(struct Foundation));
    if ( self ) {
        PileCtor((struct Pile*)self, "Foundation", slot, fan, drag, buildfunc, dragfunc);
        self->super.vtable = &foundationVtable;
        self->accept = 0; // accept any by default
    }
    return self;
}

bool FoundationCardTapped(struct Card *c)
{
    c->owner->owner->errorString[0] = '\0';
    return false;
}

bool FoundationPileTapped(struct Pile *p)
{
    p->owner->errorString[0] = '\0';
    return false;
}

bool FoundationCanAcceptTail(struct Pile *const self, lua_State *L, struct Array *const tail)
{
    if ( ArrayLen(tail) != 1 ) {
        strcpy(self->owner->errorString, "Can only move single cards to a foundation");
        return false;
    }
    if ( ArrayLen(self->cards) == 13 ) {
        strcpy(self->owner->errorString, "The foundation is full");
        return false;
    }
    if ( ArrayLen(self->cards) == 0 ) {
        struct Foundation *f = (struct Foundation*)self;
        if ( f->accept != 0 ) {
            struct Card* c = ArrayPeek(tail);
            if ( c->id.ordinal != f->accept ) {
                sprintf(self->owner->errorString, "The foundation can only accept a %d, not a %d", f->accept, c->id.ordinal);
                return false;
            }
        }
        return true;
    }
    return ConformantBuildAppend(L, self, tail);
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

void FoundationDraw(struct Pile *const self)
{
    PileDraw(self);

    struct Foundation* f = (struct Foundation*)self;
    if ( f->accept != 0 ) {
        // extern Font fontAcme;
        Vector2 pos = PileGetScreenPos(self);
        pos.x += 10;
        pos.y += 10;
        // DrawTextEx(fontAcme, ords[f->accept], pos, 16, 0, (Color){255,255,255,127});
        DrawText(UtilOrdToShortString(f->accept), (int)pos.x, (int)pos.y, 24, (Color){255,255,255,31});
    }
}
