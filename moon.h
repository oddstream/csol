/* moon.h */

#ifndef MOON_H
#define MOON_H

void MoonRegisterFunctions(lua_State* L);

// void MoonPushCardAsTable(lua_State *L, struct Card *const c);
// void MoonPushTailAsTable(lua_State *L, struct Array *const tail);
// void MoonPushArrayAsGlobalArray(lua_State *L, const char *name, struct Array *const a);

int MoonAddPile(lua_State* L);
int MoonFindPile(lua_State* L);
int MoonPileLabel(lua_State* L);
int MoonPileType(lua_State* L);
/*deprecate*/int MoonPileGet(lua_State *L);
/*deprecate*/int MoonPileLen(lua_State *L);
/*deprecate*/int MoonPilePeek(lua_State* L);
// int MoonPileDemoteCards(lua_State *L);
// int MoonPilePromoteCards(lua_State *L);
int MoonMoveCard(lua_State* L);
int MoonMoveAllCards(lua_State* L);
int MoonRefan(lua_State* L);

int MoonCardColor(lua_State* L);
int MoonCardOrdinal(lua_State* L);
int MoonCardOwner(lua_State* L);
int MoonCardProne(lua_State* L);
int MoonCardSuit(lua_State* L);
// int MoonCardToTable(lua_State *L);

/*deprecate*/int MoonTailGet(lua_State* L);
/*deprecate*/int MoonTailLen(lua_State* L);

int MoonGet(lua_State* L);
int MoonLen(lua_State* L);
int MoonFirst(lua_State* L);
int MoonLast(lua_State* L);
int MoonEmpty(lua_State* L);

int MoonCardPairs(lua_State *L);
int MoonSwapCards(lua_State *L);

int MoonToast(lua_State *L);

#endif