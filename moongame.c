/* moongame.c */

#include "exiface.h"
#include "trace.h"
#include "luautil.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static void BuildPiles(struct Baize *const baize)
{
    if (lua_getglobal(baize->L, "BuildPiles") != LUA_TFUNCTION) { // push value of "BuildPiles" onto the stack, return type
        CSOL_ERROR("%s", "BuildPiles is not a function");
        lua_pop(baize->L, 1);    // remove "BuildPiles" from stack
    } else {
        if (lua_pcall(baize->L, 0, 0, 0) != LUA_OK) {
            CSOL_ERROR("running Lua function: %s", lua_tostring(baize->L, -1));
            lua_pop(baize->L, 1);    // remove error
        } else {
            // fprintf(stderr, "BuildPiles called ok\n");
        }
    }
}

static void StartGame(struct Baize *const baize)
{
    if (lua_getglobal(baize->L, "StartGame") != LUA_TFUNCTION) {  // push Lua function name onto the stack
        CSOL_INFO("%s", "StartGame is not a function");
        lua_pop(baize->L, 1);  // remove function name
    } else {
        // no args, no returns
        if ( lua_pcall(baize->L, 0, 0, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s", lua_tostring(baize->L, -1));
            lua_pop(baize->L, 1);    // remove error
        } else {
            // nothing
        }
    }
}

static void AfterMove(struct Baize *const baize)
{
    if (lua_getglobal(baize->L, "AfterMove") != LUA_TFUNCTION) {  // push Lua function name onto the stack
        // CSOL_INFO("%s", "AfterMove is not a function");
        lua_pop(baize->L, 1);  // remove func from stack
    } else {
        // no args, no returns
        if ( lua_pcall(baize->L, 0, 0, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s", lua_tostring(baize->L, -1));
            lua_pop(baize->L, 1);    // remove error
        } else {
            // nothing
        }
    }
}

static const char* TailMoveError(struct Array *const tail)
{
    struct Card *const c0 = ArrayGet(tail, 0);
    struct Pile *const pile = CardOwner(c0);
    struct Baize *const baize = PileOwner(pile);

    const char *result = (void*)0;

    if (LuaUtilSetupTableMethod(baize->L, pile->category, "TailMoveError")) {
        lua_pushlightuserdata(baize->L, tail);
        // one arg (tail), one return (error string)
        if ( lua_pcall(baize->L, 1, 1, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(baize->L, -1));
            lua_pop(baize->L, 1);  // remove error
        } else {
            if (lua_isstring(baize->L, -1)) {
                result = lua_tostring(baize->L, -1);
            } else if (lua_isnil(baize->L, -1)) {
                ;
            } else {
                CSOL_ERROR("%s", "unexpected return");
            }
            lua_pop(baize->L, 1);
        }
    }
    return result;
}

static const char* TailAppendError(struct Pile *const pile, struct Array *const tail)
{
    struct Baize *const baize = PileOwner(pile);

    const char *result = (void*)0;

    if (LuaUtilSetupTableMethod(baize->L, pile->category, "TailAppendError")) {
        lua_pushlightuserdata(baize->L, pile);
        lua_pushlightuserdata(baize->L, tail);
        // two args (pile, tail), one return (error string)
        if ( lua_pcall(baize->L, 2, 1, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(baize->L, -1));
            lua_pop(baize->L, 1);  // remove error
        } else {
            if (lua_isstring(baize->L, -1)) {
                result = lua_tostring(baize->L, -1);
            } else if (lua_isnil(baize->L, -1)) {
                ;
            } else {
                CSOL_ERROR("%s", "unexpected return");
            }
            lua_pop(baize->L, 1);
        }
    }

    return result;
}

static const char* PileConformantError(struct Pile *const pile)
{
    struct Baize *const baize = PileOwner(pile);

    const char *result = (void*)0;

    if (LuaUtilSetupTableMethod(baize->L, pile->category, "PileConformantError")) {
        lua_pushlightuserdata(baize->L, pile);
        // one arg (pile), one return (error string)
        if ( lua_pcall(baize->L, 1, 1, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(baize->L, -1));
            lua_pop(baize->L, 1);  // remove error
        } else {
            if (lua_isstring(baize->L, -1)) {
                result = lua_tostring(baize->L, -1);
            } else if (lua_isnil(baize->L, -1)) {
                ;
            } else {
                CSOL_ERROR("%s", "unexpected return");
            }
            lua_pop(baize->L, 1);
        }
    }

    return result;
}

static void PileSortedAndUnsorted(struct Pile *const pile, int* sorted, int* unsorted)
{
    struct Baize *const baize = PileOwner(pile);

    if (LuaUtilSetupTableMethod(baize->L, pile->category, "SortedAndUnsorted")) {
        lua_pushlightuserdata(baize->L, pile);
        // one arg (pile), two returns (integers)
        if ( lua_pcall(baize->L, 1, 2, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(baize->L, -1));
            lua_pop(baize->L, 1);  // remove error
        } else {
            if ( lua_isinteger(baize->L, -2) && lua_isinteger(baize->L, -1) ) {
                int extraSorted = lua_tointeger(baize->L, -2);
                int extraUnsorted = lua_tointeger(baize->L, -1);
                *sorted += extraSorted;
                *unsorted += extraUnsorted;
            } else {
                CSOL_ERROR("%s", "expecting two numbers");
            }
            lua_pop(baize->L, 2);
        }
    }
}

static void TailTapped(struct Array *const tail)
{
    struct Card* c0 = ArrayGet(tail, 0);
    struct Pile* pile = CardOwner(c0);
    struct Baize *baize = PileOwner(pile);

    if (!LuaUtilSetupTableMethod(baize->L, pile->category, "TailTapped")) {
        CSOL_INFO("%s.TailTapped is not a function, reverting to internal default (tail len %lu)", pile->category, ArrayLen(tail));
        pile->vtable->TailTapped(pile, tail);
    } else {
        // push one arg, the tail
        lua_pushlightuserdata(baize->L, tail);
        // one arg (tail), no return (function can Toast itself for all I care)
        if ( lua_pcall(baize->L, 1, 0, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(baize->L, -1));
            lua_pop(baize->L, 1);
        }
    }
}

static void PileTapped(struct Pile *const pile)
{
    struct Baize *baize = PileOwner(pile);

    if (!LuaUtilSetupTableMethod(baize->L, pile->category, "PileTapped")) {
        CSOL_INFO("%s.PileTapped is not a function, reverting to internal default", pile->category);
        pile->vtable->PileTapped(pile);
    } else {
        // push one arg, the (non-existant) tail
        lua_pushnil(baize->L);
        // one arg (nil tail), no return (function can Toast itself for all I care)
        if ( lua_pcall(baize->L, 1, 0, 0) != LUA_OK ) {
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(baize->L, -1));
            lua_pop(baize->L, 1);
        }
    }
}

static int PercentComplete(struct Baize *const baize)
{
    int percent = 0;

    // let Lua have first dibs at this by calling function PercentComplete()
    if (lua_getglobal(baize->L, "PercentComplete") == LUA_TFUNCTION) {  // push Lua function name onto the stack
        if ( lua_pcall(baize->L, 0, 1, 0) != LUA_OK ) {  // no args, one return
            CSOL_ERROR("running Lua function: %s\n", lua_tostring(baize->L, -1));
            lua_pop(baize->L, 1);    // remove error
        } else {
            percent = lua_tointeger(baize->L, -1);
        }
    } else {
        lua_pop(baize->L, 1);    // remove "PercentComplete"

        int sorted = 0, unsorted = 0;
        size_t index;
        for ( struct Pile *p = ArrayFirst(baize->piles, &index); p; p = ArrayNext(baize->piles, &index) ) {
            p->vtable->CountSortedAndUnsorted(p, &sorted, &unsorted);
        }
        percent = (int)(UtilMapValue((float)sorted-(float)unsorted, -(float)baize->numberOfCardsInLibrary, (float)baize->numberOfCardsInLibrary, 0.0f, 100.0f));
    }

    return percent;
}

static struct ExecutionInterface moonGameVtable = {
    &BuildPiles,
    &StartGame,
    &AfterMove,
    &TailMoveError,
    &TailAppendError,
    &PileConformantError,
    &PileSortedAndUnsorted,
    &TailTapped,
    &PileTapped,
    &PercentComplete,
};

// get the interface to run a variant through a Lua script
struct ExecutionInterface* GetMoonInterface(void)
{
    return &moonGameVtable;
}