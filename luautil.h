/* luautil.h */

#ifndef LUAUTIL_H
#define LUAUTIL_H

#include "baize.h"

void OpenLua(struct Baize *const baize);
void CloseLua(void);

_Bool LuaUtilGetGlobalBool(const char* var, const _Bool def);
int LuaUtilGetGlobalInt(const char* var, const int def);
float LuaUtilGetGlobalFloat(const char* var, const float def);
const char* LuaUtilGetGlobalString(const char* var, const char* def);
float LuaUtilGetFieldFloat(const char* key, const float def);

_Bool LuaUtilSetupTableMethod(const char *table, const char *method);

#endif
