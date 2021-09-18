/* conformant.h */

#ifndef CONFORMANT_H
#define CONFORMANT_H

#include <stdio.h>
#include "array.h"
#include "pile.h"

bool ConformantBuildTail(lua_State *L, struct Pile *const pile, struct Array *tail);
bool ConformantBuildAppend(lua_State *L, struct Pile *const pile, struct Array *tail);
bool ConformantDragTail(lua_State *L, struct Pile *const pile, struct Array *tail);
bool CheckDrag(struct Array* tail);

#endif