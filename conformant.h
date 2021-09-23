/* conformant.h */

#ifndef CONFORMANT_H
#define CONFORMANT_H

#include <stdio.h>
#include "array.h"
#include "pile.h"

bool ConformantBuild(lua_State *L, struct Pile *const pile, struct Card *c, struct Array *tail);
bool ConformantDrag(lua_State *L, struct Pile *const pile, struct Array *tail);

#endif