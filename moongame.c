/* moongame.c */

#include "script.h"
#include "trace.h"
#include "luautil.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

extern lua_State *L;

static void BuildPiles(struct Baize *const baize)
{
    (void)baize;

    if (lua_getglobal(L, "BuildPiles") != LUA_TFUNCTION) { // push value of "BuildPiles" onto the stack, return type
        CSOL_ERROR("%s", "BuildPiles is not a function");
        lua_pop(L, 1);    // remove "BuildPiles" from stack
    } else {
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            CSOL_ERROR("running Lua function: %s", lua_tostring(L, -1));
            lua_pop(L, 1);    // remove error
        } else {
            // fprintf(stderr, "BuildPiles called ok\n");
        }
    }
}

static void StartGame(struct Baize *const baize)
{
    (void)baize;

    if (lua_getglobal(L, "StartGame") != LUA_TFUNCTION) {  // push Lua function name onto the stack
        CSOL_INFO("%s", "StartGame is not a function");
        lua_pop(L, 1);  // remove function name
    } else {
        // no args, no returns
        if ( lua_pcall(L, 0, 0, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s", lua_tostring(L, -1));
            lua_pop(L, 1);    // remove error
        } else {
            // nothing
        }
    }
}

static void AfterMove(struct Baize *const baize)
{
    (void)baize;

    if (lua_getglobal(L, "AfterMove") != LUA_TFUNCTION) {  // push Lua function name onto the stack
        // CSOL_INFO("%s", "AfterMove is not a function");
        lua_pop(L, 1);  // remove func from stack
    } else {
        // no args, no returns
        if ( lua_pcall(L, 0, 0, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s", lua_tostring(L, -1));
            lua_pop(L, 1);    // remove error
        } else {
            // nothing
        }
    }
}

static const char* TailMoveError(struct Array *const tail)
{
    struct Card *const c0 = ArrayGet(tail, 0);
    struct Pile *const pile = CardOwner(c0);

    const char *result = (void*)0;

    if (LuaUtilSetupTableMethod(pile->category, "TailMoveError")) {
        lua_pushlightuserdata(L, tail);
        // one arg (tail), one return (error string)
        if ( lua_pcall(L, 1, 1, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);  // remove error
        } else {
            if (lua_isstring(L, -1)) {
                result = lua_tostring(L, -1);
            } else if (lua_isnil(L, -1)) {
                ;
            } else {
                CSOL_ERROR("%s", "unexpected return");
            }
            lua_pop(L, 1);
        }
    }
    return result;
}

static const char* TailAppendError(struct Pile *const pile, struct Array *const tail)
{
    const char *result = (void*)0;

    if (LuaUtilSetupTableMethod(pile->category, "TailAppendError")) {
        lua_pushlightuserdata(L, pile);
        lua_pushlightuserdata(L, tail);
        // two args (pile, tail), one return (error string)
        if ( lua_pcall(L, 2, 1, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);  // remove error
        } else {
            if (lua_isstring(L, -1)) {
                result = lua_tostring(L, -1);
            } else if (lua_isnil(L, -1)) {
                ;
            } else {
                CSOL_ERROR("%s", "unexpected return");
            }
            lua_pop(L, 1);
        }
    }

    return result;
}

static int PileUnsortedPairs(struct Pile *const pile)
{
    int unsorted = 0;

    if (LuaUtilSetupTableMethod(pile->category, "UnsortedPairs")) {
        lua_pushlightuserdata(L, pile);
        // one arg (pile), one return (integer)
        if ( lua_pcall(L, 1, 1, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);  // remove error
        } else {
            if (lua_isinteger(L, -1)) {
                unsorted = lua_tointeger(L, -1);
            } else {
                CSOL_ERROR("%s", "expecting an integer");
            }
            lua_pop(L, 1);
        }
    }
    return unsorted;
}

static void TailTapped(struct Array *const tail)
{
    struct Card* c0 = ArrayGet(tail, 0);
    struct Pile* pile = CardOwner(c0);

    if (!LuaUtilSetupTableMethod(pile->category, "TailTapped")) {
        pile->vtable->TailTapped(tail);
    } else {
        // push one arg, the tail
        lua_pushlightuserdata(L, tail);
        // one arg (tail), no return (function can Toast itself for all I care)
        if ( lua_pcall(L, 1, 0, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
}

static void PileTapped(struct Pile *const pile)
{
    if (LuaUtilSetupTableMethod(pile->category, "PileTapped")) {
        // push one arg, the (non-existant) tail
        lua_pushnil(L);
        // one arg (nil tail), no return (function can Toast itself for all I care)
        if ( lua_pcall(L, 1, 0, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
}

static int PercentComplete(struct Baize *const baize)
{
    int percent = 0;

    // let Lua have first dibs at this by calling function PercentComplete()
    if (lua_getglobal(L, "PercentComplete") == LUA_TFUNCTION) {  // push Lua function name onto the stack
        if ( lua_pcall(L, 0, 1, 0) != LUA_OK ) {  // no args, one return
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);    // remove error
        } else {
            if (lua_isinteger(L, -1)) {
                percent = lua_tointeger(L, -1);
            } else {
                CSOL_ERROR("%s", "integer expected");
                lua_pop(L, 1);
            }
        }
    } else {
        lua_pop(L, 1);    // remove "PercentComplete"

        int pairs = 0, unsorted = 0;
        size_t index;
        for ( struct Pile *pile = ArrayFirst(baize->piles, &index); pile; pile = ArrayNext(baize->piles, &index) ) {
            if (PileLen(pile) > 1) {
                pairs += PileLen(pile) - 1;
            }
            unsorted += pile->vtable->UnsortedPairs(pile);
        }
        // CSOL_INFO("pairs:%d unsorted:%d", pairs, unsorted);
        percent = (int)(100.0f - UtilMapValue((float)unsorted, 0, (float)pairs, 0.0f, 100.0f));
    }

    return percent;
}

static const char* Wikipedia(void)
{
    const char *str = (void*)0;

    if (lua_getglobal(L, "Wikipedia") == LUA_TFUNCTION) {  // push Lua function name onto the stack
        if ( lua_pcall(L, 0, 1, 0) != LUA_OK ) {  // no args, one return
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);    // remove error
        } else {
            str = lua_tostring(L, -1);
        }
    }

    return str;
}

static struct ScriptInterface moonGameVtable = {
    &BuildPiles,
    &StartGame,
    &AfterMove,
    &TailMoveError,
    &TailAppendError,
    &PileUnsortedPairs,
    &TailTapped,
    &PileTapped,
    &PercentComplete,
    &Wikipedia,
};

// get the interface to run a variant through a Lua script
struct ScriptInterface* GetMoonGameInterface(void)
{
    return &moonGameVtable;
}