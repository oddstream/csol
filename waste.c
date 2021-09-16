/* waste.c */

#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <lua.h>

#include "pile.h"
#include "array.h"
#include "waste.h"

static struct PileVtable wasteVtable = {
    &WasteCardTapped,
    &WastePileTapped,
    &WasteCanAcceptTail,
    &WasteSetAccept,

    &PileUpdate,
    &PileDraw,
    &PileFree,
};

struct Waste* WasteNew(Vector2 pos, enum FanType fan, const char* buildfunc, const char* dragfunc)
{
    struct Waste* self = malloc(sizeof(struct Waste));
    if ( self ) {
        PileCtor((struct Pile*)self, "Waste", pos, fan, buildfunc, dragfunc);
        self->super.vtable = &wasteVtable;
    }
    return self;
}

void WasteCardTapped(lua_State *L, struct Card *c)
{
    (void)L;
    (void)c;
}

void WastePileTapped(lua_State *L, struct Pile *p)
{
    (void)L;
    (void)p;
}

bool WasteCanAcceptTail(struct Pile *const self, lua_State *L, struct Array *const tail)
{
    // only allow cards moved from Stock
    (void)self;
    (void)L;
    (void)tail;
    return false;
}

void WasteSetAccept(struct Pile *const self, enum CardOrdinal ord)
{
    // we don't do that here
    (void)self;
    (void)ord;
}
