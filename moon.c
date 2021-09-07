/* moon.c */

#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>

#include "moon.h"

int MoonGetGlobalInt(lua_State* L, const char* var, const int def) {
    int result = def;
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    int typ = lua_getglobal(L, var);    // pushes value onto stack
    if ( typ != LUA_TNUMBER ) {
        fprintf(stderr, "%s is not a number\n", var);
        result = def;
    } else {
        int isnum;
        result = lua_tointegerx(L, -1, &isnum); // does not alter stack
        if ( !isnum ) {
            fprintf(stderr, "%s cannot be converted to an integer\n", var);
            result = def;
        }
    }
    lua_pop(L, 1); // remove integer from stack
    // if ( lua_gettop(L) ) {
    //     lua_pop(L, 1);
    // }
    // lua_settop(L, 0);
    fprintf(stderr, "%s=%d\n", var, result);
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    return result;
}

float MoonGetFieldNumber(lua_State* L, const char* key, const float def) {
    // assumes table is on top of stack
    float result = def;
    int isnum;
    lua_getfield(L, -1, key);    // pushes onto the stack the value t[k], where t is the value at the given index
    result = (float)lua_tonumberx(L, -1, &isnum);    // returns a lua_Number
    if ( !isnum ) {
        fprintf(stderr, "%s is not a number\n", key);
        result = def;
    }
    lua_pop(L, 1);  // remove number
    return result;
}
