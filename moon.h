/* moon.h */

#ifndef MOON_H
#define MOON_H

#include "array.h"
#include "card.h"

void MoonRegisterFunctions(lua_State* L);

int MoonGetGlobalInt(lua_State* L, const char* var, const int def);
float MoonGetGlobalFloat(lua_State* L, const char* var, const float def);
const char* MoonGetGlobalString(lua_State* L, const char* var, const char* def);
float MoonGetFieldNumber(lua_State* L, const char* key, const float def);
void MoonPushCard(lua_State *L, struct Card *const c);
void MoonPushTail(lua_State *L, struct Array *const tail);

int MoonAddPile(lua_State* L);
int MoonDealUp(lua_State* L);
int MoonDealDown(lua_State* L);
int MoonFindPile(lua_State* L);
int MoonMovePileTo(lua_State* L);
int MoonGetCategory(lua_State* L);
int MoonGetCardCount(lua_State *L);
int MoonSetAccept(lua_State* L);
int MoonSetRecycles(lua_State* L);
int MoonMoveCard(lua_State* L);

#endif