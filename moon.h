/* moon.h */

#ifndef MOON_H
#define MOON_H

int MoonGetGlobalInt(lua_State * L, const char* var, const int def);
float MoonGetFieldNumber(lua_State* L, const char* key, const float def);

#endif