/* constraint.c */

#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "array.h"
#include "baize.h"
#include "constraint.h"
#include "luautil.h"
#include "moon.h"

static bool getBoolStringReturn(struct Baize *const baize, const char *func)
{
    // lua_pcall(L, n, 2, 0) ensures there are two things on the stack afterwards, even if they are nil
    // if last element (-1) is a string or nil
    // previous element (-2) should be a boolean

    lua_State *L = baize->L;
    bool result = false;

    if ( lua_isstring(L, -1) ) {
        BaizeSetError(baize, lua_tostring(L, -1));
        if ( lua_isboolean(L, -2) ) {
            result = lua_toboolean(L, -2);
        } else {
            fprintf(stderr, "WARNING: expecting boolean return from %s, instead got a %d\n", func, lua_type(L, -2));
        }
    } else if ( lua_isnil(L, -1) ) {
        if ( lua_isboolean(L, -2) ) {
            result = lua_toboolean(L, -2);
        } else {
            fprintf(stderr, "WARNING: expecting boolean return from %s, instead got a %d\n", func, lua_type(L, -2));
        }
    } else {
        fprintf(stderr, "ERROR: unexpected return from %s, got types %d and %d\n", func, lua_type(L, -1), lua_type(L, -1));
    }
    lua_pop(L, 2);
    return result;
}

bool CanTailBeMoved(struct Array *const tail)
{
    struct Card *const c0 = ArrayGet(tail, 0);
    if (!CardValid(c0)) {
        fprintf(stderr, "ERROR: %s: passed invalid card\n", __func__);
        return false;
    }

    // BaizeTouchStop() checks if any tail cards are prone

    struct Pile *const pile = c0->owner;
    struct Baize *const baize = pile->owner;

    bool result = true;
    lua_State *L = baize->L;

    if (LuaUtilSetupTableMethod(L, pile->category, __func__)) {
        lua_pushlightuserdata(L, tail);
        // one arg (tail), two returns (boolean, error string)
        if ( lua_pcall(L, 1, 2, 0) != LUA_OK ) {
            fprintf(stderr, "ERROR: %s: running Lua function: %s\n", __func__, lua_tostring(L, -1));
            lua_pop(L, 1);  // remove error
        } else {
            result = getBoolStringReturn(baize, __func__);
        }
    }
    return result;
}

bool CanTailBeAppended(struct Pile *const pile, struct Array *const tail)
{
    if (!PileValid(pile)) {
        fprintf(stderr, "ERROR: %s: passed invalid pile\n", __func__);
        return false;
    }
    if (!tail) {
        fprintf(stderr, "ERROR: %s: passed invalid tail\n", __func__);
        return false;
    }

    bool result = true;
    lua_State *L = pile->owner->L;
    if (LuaUtilSetupTableMethod(L, pile->category, __func__)) {
        lua_pushlightuserdata(L, pile);
        lua_pushlightuserdata(L, tail);
        // two args (pile, tail), two returns (boolean, error string)
        if ( lua_pcall(L, 2, 2, 0) != LUA_OK ) {
            fprintf(stderr, "ERROR: %s: running Lua function: %s\n", __func__, lua_tostring(L, -1));
            lua_pop(L, 1);  // remove error
        } else {
            result = getBoolStringReturn(pile->owner, __func__);
        }
    }
    return result;
}

bool IsPileConformant(struct Pile *const pile)
{
    if (!PileValid(pile)) {
        fprintf(stderr, "ERROR: %s: passed invalid pile\n", __func__);
        return false;
    }


    bool result = true;
    lua_State *L = pile->owner->L;

    if (LuaUtilSetupTableMethod(L, pile->category, __func__)) {
        lua_pushlightuserdata(L, pile);
        // one arg (pile), two returns (boolean, error string)
        if ( lua_pcall(L, 1, 2, 0) != LUA_OK ) {
            fprintf(stderr, "ERROR: %s: running Lua function: %s\n", __func__, lua_tostring(L, -1));
            lua_pop(L, 1);  // remove error
        } else {
            result = getBoolStringReturn(pile->owner, __func__);
        }
    }
    return result;
}
