/* foundation.h */

#ifndef FOUNDATION_H
#define FOUNDATION_H

#include <stdlib.h>
#include <raylib.h>
#include "array.h"
#include "card.h"
#include "pile.h"

struct Foundation {
    struct Pile super;
    enum CardOrdinal accept;
};

struct Foundation* FoundationNew(Vector2 pos, enum FanType fan, const char* buildfunc, const char* dragfunc);
void FoundationCardTapped(lua_State *L, struct Card *c);
void FoundationPileTapped(lua_State *L, struct Pile *p);
bool FoundationCanAcceptTail(struct Pile *const self, lua_State *L, struct Array *const tail);
void FoundationSetAccept(struct Pile *const self, enum CardOrdinal ord);
void FoundationDraw(struct Pile *const self);

#endif
