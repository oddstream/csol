/* luautil.h */

#ifndef LUAUTIL_H
#define LUAUTIL_H

_Bool LuaUtilGetGlobalBool(lua_State* L, const char* var, const _Bool def);
int LuaUtilGetGlobalInt(lua_State* L, const char* var, const int def);
float LuaUtilGetGlobalFloat(lua_State* L, const char* var, const float def);
const char* LuaUtilGetGlobalString(lua_State* L, const char* var, const char* def);
float LuaUtilGetFieldFloat(lua_State* L, const char* key, const float def);

_Bool LuaUtilSetupTableMethod(lua_State *L, const char *table, const char *method);

#endif
