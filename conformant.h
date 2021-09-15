/* conformant.h */

#ifndef CONFORMANT_H
#define CONFORMANT_H

#include <stdio.h>
#include "array.h"
#include "pile.h"

bool ConformantBuildTail(lua_State *L, struct Pile *const pile, struct Array* tail);
bool ConformantBuildPair(lua_State *L, struct Pile *const pile, struct Card* c0, struct Card* c1);
bool ConformantDragTail(lua_State *L, struct Pile *const pile, struct Array* tail);
// bool ConformantDragPair(lua_State *L, struct Pile *const pile, struct Card* c0, struct Card* c1);

#endif