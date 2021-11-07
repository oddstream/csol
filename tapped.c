/* tapped.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <raylib.h>
#include <lua.h>

#include "baize.h"
#include "luautil.h"

void BaizeTailTapped(struct Baize *const self)
{
    if (!self->tail || ArrayLen(self->tail)==0) {
        fprintf(stderr, "ERROR: %s: passed nil tail\n", __func__);
    }

    lua_State *L = self->L;
    struct Card* c0 = ArrayGet(self->tail, 0);
    struct Pile* pile = CardOwner(c0);

    if (!LuaUtilSetupTableMethod(L, pile->category, "TailTapped")) {
        fprintf(stderr, "INFO: %s: %s.TailTapped is not a function, reverting to internal default (tail len %lu)\n", __func__, pile->category, ArrayLen(self->tail));
        pile->vtable->TailTapped(pile, self->tail);
    } else {
        // push one arg, the tail
        lua_pushlightuserdata(L, self->tail);
        // one arg (tail), no return (function can Toast itself for all I care)
        if ( lua_pcall(L, 1, 0, 0) != LUA_OK ) {
            fprintf(stderr, "ERROR: %s: running Lua function: %s\n", __func__, lua_tostring(self->L, -1));
            lua_pop(L, 1);
        }
    }
}

void BaizePileTapped(struct Baize *const self, struct Pile *const pile)
{
    lua_State *L = self->L;

    if (!LuaUtilSetupTableMethod(L, pile->category, "PileTapped")) {
        fprintf(stderr, "INFO: %s: %s.PileTapped is not a function, reverting to internal default\n", __func__, pile->category);
        pile->vtable->PileTapped(pile);
    } else {
        // push one arg, the (non existant) tail
        lua_pushnil(L);
        // one arg (nil tail), no return (function can Toast itself for all care)
        if ( lua_pcall(L, 1, 0, 0) != LUA_OK ) {
            fprintf(stderr, "ERROR: %s: error running Lua function: %s\n", __func__, lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
}

#if 0
// ask a Lua function if these cards match in some way
_Bool BaizeMatchCards(struct Baize *const self, struct Card *const c1, struct Card *const c2)
{
    lua_State *L = self->L;
    _Bool result = 0;

    if (lua_getglobal(L, "MatchCards") != LUA_TFUNCTION) {
        fprintf(stderr, "WARNING: %s: MatchCards is not a function\n", __func__);
        lua_pop(L, 1);  // pop whatever "MatchCards" is
    } else {
        lua_pushlightuserdata(L, c1);
        lua_pushlightuserdata(L, c2);
        // two args, one bool return
        if ( lua_pcall(L, 2, 1, 0) != LUA_OK ) {
            fprintf(stderr, "ERROR: %s: running Lua function: %s\n", __func__, lua_tostring(L, -1));
            lua_pop(L, 1);  // pop error
        } else {
            if (lua_isboolean(L, -1)) {
                result = lua_toboolean(L, -1);
            } else {
                fprintf(stderr, "ERROR: %s: expecting boolean return\n", __func__);
                lua_pop(L, 1);  // pop whatever that is
            }
        }
    }

    return result;
}
#endif