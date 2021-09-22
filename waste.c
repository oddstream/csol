/* waste.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "waste.h"

static struct PileVtable wasteVtable = {
    &WasteCardTapped,
    &WastePileTapped,
    &WasteCanAcceptTail,
    &WasteSetAccept,
    &WasteSetRecycles,

    &PileUpdate,
    &PileDraw,
    &PileFree,
};

struct Waste* WasteNew(Vector2 slot, enum FanType fan, const char* buildfunc, const char* dragfunc)
{
    struct Waste* self = malloc(sizeof(struct Waste));
    if ( self ) {
        PileCtor((struct Pile*)self, "Waste", slot, fan, buildfunc, dragfunc);
        self->super.vtable = &wasteVtable;
    }
    return self;
}

bool WasteCardTapped(struct Card *c)
{
    c->owner->owner->errorString[0] = '\0';
    return false;
}

bool WastePileTapped(struct Pile *p)
{
    p->owner->errorString[0] = '\0';
    return false;
}

bool WasteCanAcceptTail(struct Pile *const self, lua_State *L, struct Array *const tail)
{
    // TODO maybe move three cards
    if ( ArrayLen(tail) == 1 ) {
        struct Card *c = ArrayGet(tail, 0);
        if ( c && c->owner == self->owner->stock ) {
            return true;
        } else {
            strcpy(self->owner->errorString, "You can only move cards to a Waste pile from the Stock");
        }
    } else {
        strcpy(self->owner->errorString, "You can only move one card to a Waste pile");
    }
    (void)L;
    return false;
}

void WasteSetAccept(struct Pile *const self, enum CardOrdinal ord)
{
    // we don't do that here
    (void)self;
    (void)ord;
}

void WasteSetRecycles(struct Pile *const self, int r)
{
    // we don't do that here
    (void)self;
    (void)r;
}
