/* tapped.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <raylib.h>
#include <lua.h>

#include "baize.h"
#include "luautil.h"

void BaizeGetLuaGlobals(struct Baize *const self)
{
    if (!self->stock) {
        fprintf(stderr, "ERROR: %s: Baize not formed\n", __func__);
        exit(666);
        return;
    }
    self->powerMoves = LuaUtilGetGlobalBool(self->L, "POWER_MOVES", false);
    self->stock->vtable->SetRecycles(self->stock, LuaUtilGetGlobalInt(self->L, "STOCK_RECYCLES", 32767));
}

void BaizeStartGame(struct Baize *const self)
{
    unsigned crc = BaizeCRC(self);

    if (lua_getglobal(self->L, "StartGame") != LUA_TFUNCTION) {  // push Lua function name onto the stack
        fprintf(stderr, "INFO: %s: StartGame is not a function\n", __func__);
        lua_pop(self->L, 1);  // remove function name
    } else {
        // no args, no returns
        if ( lua_pcall(self->L, 0, 0, 0) != LUA_OK ) {
            fprintf(stderr, "ERROR: %s: running Lua function: %s\n", __func__, lua_tostring(self->L, -1));
            lua_pop(self->L, 1);    // remove error
        } else {
            // nothing
        }
    }

    if (BaizeCRC(self) != crc) {
        fprintf(stdout, "INFO: %s: StartGame has changed the baize\n", __func__);
    }
}

void BaizeTailTapped(struct Baize *const self)
{
    if (!self->tail || ArrayLen(self->tail)==0) {
        fprintf(stderr, "ERROR: %s: passed nil tail\n", __func__);
    }

    lua_State *L = self->L;
    struct Card* c0 = ArrayGet(self->tail, 0);
    struct Pile* pile = CardOwner(c0);

    if (!LuaUtilSetupTableMethod(L, pile->category, "Tapped")) {
        fprintf(stderr, "INFO: %s: %s.Tapped is not a function, reverting to internal default (tail len %lu)\n", __func__, pile->category, ArrayLen(self->tail));
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

    if (!LuaUtilSetupTableMethod(L, pile->category, "Tapped")) {
        fprintf(stderr, "INFO: %s: %s.Tapped is not a function, reverting to internal default (nil tail)\n", __func__, pile->category);
        pile->vtable->TailTapped(pile, NULL);
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
