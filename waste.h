/* waste.h */

#ifndef WASTE_H
#define WASTE_H

#include <stdlib.h>
#include <raylib.h>
#include <lua.h>
#include "array.h"
#include "card.h"
#include "pile.h"

struct Waste {
    struct Pile super;
};

struct Waste* WasteNew(Vector2 pos, enum FanType fan, const char* buildfunc, const char* dragfunc);
void WasteCardTapped(lua_State *L, struct Card *c);
void WastePileTapped(lua_State *L, struct Pile *p);
bool WasteCanAcceptTail(struct Pile *const self, lua_State *L, struct Array *const tail);
void WasteSetAccept(struct Pile *const self, enum CardOrdinal ord);
void WasteDraw(struct Pile *const self);

#endif
