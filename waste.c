/* waste.c */

#include <stdlib.h>
#include <stdio.h>
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

struct Waste* WasteNew(Vector2 slot, enum FanType fan, enum DragType drag, const char* buildfunc, const char* dragfunc)
{
    struct Waste* self = malloc(sizeof(struct Waste));
    if ( self ) {
        PileCtor((struct Pile*)self, "Waste", slot, fan, drag, buildfunc, dragfunc);
        self->super.vtable = &wasteVtable;
    }
    return self;
}

void WasteCardTapped(struct Card *c)
{
    (void)c;
}

void WastePileTapped(struct Pile *p)
{
    (void)p;
}

bool WasteCanAcceptTail(struct Pile *const self, lua_State *L, struct Array *const tail)
{
    // TODO maybe move three cards
    if ( ArrayLen(tail) == 1 ) {
        struct Card *c = ArrayGet(tail, 0);
        if ( c && c->owner == self->owner->stock ) {
            return true;
        }
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
