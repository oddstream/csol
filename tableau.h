/* tableau.h */

#ifndef TABLEAU_H
#define TABLEAU_H

#include <stdlib.h>
#include <raylib.h>
#include <lua.h>
#include "array.h"
#include "card.h"
#include "pile.h"

struct Tableau {
    struct Pile super;
    enum CardOrdinal accept;
};

struct Tableau* TableauNew(Vector2 pos, enum FanType fan, const char* buildfunc, const char* dragfunc);
void TableauCardTapped(lua_State *L, struct Card *c);
void TableauPileTapped(lua_State *L, struct Pile *p);
bool TableauCanAcceptTail(struct Pile *const self, lua_State *L, struct Array *const tail);
void TableauSetAccept(struct Pile *const self, enum CardOrdinal ord);
void TableauDraw(struct Pile *const self);

#endif
