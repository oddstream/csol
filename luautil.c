/* luautil.c */

#include <stdlib.h>
#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luautil.h"

_Bool LuaUtilGetGlobalBool(lua_State* L, const char* var, const _Bool def)
{
    _Bool result = def;
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    int typ = lua_getglobal(L, var);    // pushes value onto stack
    if ( typ == LUA_TNIL ) {
        // fprintf(stderr, "%s is nil\n", var);
        result = def;
    } else if ( typ != LUA_TBOOLEAN ) {
        fprintf(stderr, "ERROR: %s: %s is not a _Boolean\n", __func__, var);
        result = def;
    } else {
        result = lua_toboolean(L, -1); // does not alter stack
    }
    lua_pop(L, 1); // remove _Boolean from stack
    // fprintf(stderr, "%s=%d\n", var, result);
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    return result;
}

int LuaUtilGetGlobalInt(lua_State* L, const char* var, const int def)
{
    int result = def;
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    int typ = lua_getglobal(L, var);    // pushes value onto stack
    if ( typ == LUA_TNIL ) {
        // fprintf(stderr, "%s is nil\n", var);
        result = def;
    } else if ( typ != LUA_TNUMBER ) {
        fprintf(stderr, "%s is not a number\n", var);
        result = def;
    } else {
        int isnum;
        result = (int)lua_tointegerx(L, -1, &isnum); // does not alter stack
        if ( !isnum ) {
            fprintf(stderr, "ERROR: %s: %s cannot be converted to an integer\n", __func__, var);
            result = def;
        }
    }
    lua_pop(L, 1); // remove integer from stack
    // fprintf(stderr, "%s=%d\n", var, result);
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    return result;
}

float LuaUtilGetGlobalFloat(lua_State* L, const char* var, const float def)
{
    float result = def;
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    int typ = lua_getglobal(L, var);    // pushes value onto stack
    if ( typ == LUA_TNIL ) {
        // fprintf(stderr, "%s is nil\n", var);
        result = def;
    } else if ( typ != LUA_TNUMBER ) {
        fprintf(stderr, "%s is not a number\n", var);
        result = def;
    } else {
        int isnum;
        result = (float)lua_tonumberx(L, -1, &isnum); // does not alter stack
        if ( !isnum ) {
            fprintf(stderr, "ERROR: %s: %s cannot be converted to a number\n", __func__, var);
            result = def;
        }
    }
    lua_pop(L, 1); // remove integer from stack
    // fprintf(stderr, "%s=%f\n", var, result);
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    return result;
}

const char* LuaUtilGetGlobalString(lua_State* L, const char* var, const char* def)
{
    const char* result = def;
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    int typ = lua_getglobal(L, var);    // pushes value onto stack
    if ( typ != LUA_TSTRING ) {
        fprintf(stderr, "%s is not a string\n", var);
        result = def;
    } else {
        result = lua_tostring(L, -1); // does not alter stack
        if ( !result ) {
            fprintf(stderr, "ERROR: %s: %s cannot be converted to a string\n", __func__, var);
            result = def;
        }
    }
    lua_pop(L, 1); // remove integer from stack
    // fprintf(stderr, "%s=%s\n", var, result);
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    return result;
}

float LuaUtilGetFieldFloat(lua_State* L, const char* key, const float def)
{
    // assumes table is on top of stack
    float result = def;
    int isnum;
    int  typ = lua_getfield(L, -1, key);    // pushes onto the stack the value t[k], where t is the value at the given index
    if ( typ == LUA_TNIL ) {
        // fprintf(stderr, "%s is nil\n", key);
        result = def;
    } else if ( typ != LUA_TNUMBER ) {
        fprintf(stderr, "ERROR: %s: %s is not a number\n", __func__, key);
        result = def;
    } else {
        result = (float)lua_tonumberx(L, -1, &isnum);    // returns a lua_Number
        if ( !isnum ) {
            fprintf(stderr, "ERROR: %s: %s cannot be converted to a number\n", __func__, key);
            result = def;
        }
    }
    lua_pop(L, 1);  // remove number
    return result;
}

_Bool LuaUtilSetupTableMethod(lua_State *L, const char *table, const char *method)
{
    int typ = lua_getglobal(L, table);
    if (typ != LUA_TTABLE) {
        fprintf(stderr, "INFO: %s: %s is not a table\n", __func__, table);
        lua_pop(L, 1);  // remove table name
        return 0;
    }
    typ = lua_getfield(L, -1, method);
    if (typ != LUA_TFUNCTION) {
        fprintf(stderr, "INFO: %s: %s.%s is not a function\n", __func__, table, method);
        lua_pop(L, 2);  // remove table and method names
        return 0;
    }
    return 1;
}
