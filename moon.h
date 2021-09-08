/* moon.h */

#ifndef MOON_H
#define MOON_H

void MoonRegisterFunctions(lua_State* L);

int MoonGetGlobalInt(lua_State* L, const char* var, const int def);
float MoonGetFieldNumber(lua_State* L, const char* key, const float def);

int MoonAddPile(lua_State* L);
int MoonDeal(lua_State* L);
int MoonFindPile(lua_State* L);

#endif