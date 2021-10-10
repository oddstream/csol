/* moon.h */

#ifndef MOON_H
#define MOON_H

#include "array.h"
#include "card.h"

void MoonRegisterFunctions(lua_State* L);

bool MoonGetGlobalBool(lua_State* L, const char* var, const bool def);
int MoonGetGlobalInt(lua_State* L, const char* var, const int def);
float MoonGetGlobalFloat(lua_State* L, const char* var, const float def);
const char* MoonGetGlobalString(lua_State* L, const char* var, const char* def);
float MoonGetFieldNumber(lua_State* L, const char* key, const float def);
void MoonPushCardAsTable(lua_State *L, struct Card *const c);
void MoonPushTailAsTable(lua_State *L, struct Array *const tail);

int MoonAddPile(lua_State* L);
int MoonFindPile(lua_State* L);
int MoonPileMoveTo(lua_State* L);
int MoonPileType(lua_State* L);
int MoonPileCardCount(lua_State *L);
int MoonCardOwner(lua_State* L);
int MoonSetCardProne(lua_State* L);
int MoonSetPileAccept(lua_State* L);
int MoonSetPileDraggable(lua_State* L);
int MoonSetPileRecycles(lua_State* L);
int MoonPilePeekCard(lua_State* L);
int MoonPileDemoteCards(lua_State *L);
int MoonPilePromoteCards(lua_State *L);
int MoonMoveCard(lua_State* L);
int MoonCardToTable(lua_State *L);

#endif