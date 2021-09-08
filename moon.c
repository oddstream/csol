/* moon.c */

#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>

#include "baize.h"
#include "array.h"
#include "moon.h"

struct FunctionToRegister {
    char luaFunction[32];
    lua_CFunction cFunction;
};

static struct FunctionToRegister FunctionsToRegister[] = {
    {"AddPile", MoonAddPile},
    {"Deal", MoonDeal},
    {"FindPile", MoonFindPile},
};

static struct Baize* getBaize(lua_State* L) {
    int typ = lua_getglobal(L, "BAIZE");    // push light userdata on the stack
    if ( typ != LUA_TLIGHTUSERDATA ) {
        fprintf(stderr, "global BAIZE is not light userdata\n");
    }
    struct Baize* baize = lua_touserdata(L, -1); // doesn't alter stack
    lua_pop(L, 1);  // pop light userdata
    if ( !BaizeValid(baize) ) {
        fprintf(stderr, "global BAIZE is not valid");
    }
    return baize;
}

void MoonRegisterFunctions(lua_State* L) {
    for ( size_t i=0; i<sizeof(FunctionsToRegister) / sizeof(struct FunctionToRegister); i++ ) {
        lua_pushcfunction(L, FunctionsToRegister[i].cFunction);
        lua_setglobal(L, FunctionsToRegister[i].luaFunction);
    }
}

int MoonGetGlobalInt(lua_State* L, const char* var, const int def) {
    int result = def;
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    int typ = lua_getglobal(L, var);    // pushes value onto stack
    if ( typ != LUA_TNUMBER ) {
        fprintf(stderr, "%s is not a number\n", var);
        result = def;
    } else {
        int isnum;
        result = (int)lua_tointegerx(L, -1, &isnum); // does not alter stack
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
    int  typ = lua_getfield(L, -1, key);    // pushes onto the stack the value t[k], where t is the value at the given index
    if ( typ != LUA_TNUMBER ) {
        fprintf(stderr, "%s is not a number\n", key);
    } else {
        result = (float)lua_tonumberx(L, -1, &isnum);    // returns a lua_Number
        if ( !isnum ) {
            fprintf(stderr, "%s is not a number\n", key);
            result = def;
        }
    }
    lua_pop(L, 1);  // remove number
    return result;
}

int MoonAddPile(lua_State* L) {
    struct Baize* baize = getBaize(L);

    const char* class = lua_tostring(L, 1); // doesn't alter stack
    float x = lua_tonumber(L, 2); // doesn't alter stack
    float y = lua_tonumber(L, 3); // doesn't alter stack
    enum FanType fan = lua_tointeger(L, 4); // doesn't alter stack

    // fprintf(stderr, "PileNew(%s,%f,%f,%d)\n", class, x, y, fan);

    struct Pile* p = PileNew(class, (Vector2){x, y}, fan);
    ArrayPush(&baize->piles, (void**)p);
    lua_pushlightuserdata(L, p);

    return 1;   // number of args pushed
}

int MoonDeal(lua_State* L) {
    struct Baize* baize = getBaize(L);

    struct Pile* p = lua_touserdata(L, 1); // get argument
    if ( !PileValid(p) ) {
        fprintf(stderr, "destination pile is not valid");
        return 0;
    }

    struct Card* c = PilePop(baize->stock);
    if ( c ) {
        PilePush(p, c);
    } else {
        fprintf(stderr, "cannot pop card\n");
    }

    return 0;
}

int MoonFindPile(lua_State* L) {
    struct Baize* baize = getBaize(L);

    const char* class = lua_tostring(L, 1); // doesn't alter stack
    int n = lua_tointeger(L, 2); // doesn't alter stack

    int savedPos;
    struct Pile* p = (struct Pile*)ArrayFirst(&baize->piles, &savedPos);
    while ( p ) {
        if ( strcmp(p->class, class) == 0 ) {
            n--;
            if ( n == 0 ) {
                lua_pushlightuserdata(L, p);
                return 1;
            }
        }
        p = (struct Pile*)ArrayNext(&baize->piles, &savedPos);
    }

    return 0;
}