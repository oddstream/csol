/* tapped.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <raylib.h>
#include <lua.h>

#include "baize.h"
#include "moon.h"

void BaizeGetLuaGlobals(struct Baize *const self)
{
    self->powerMoves = MoonGetGlobalBool(self->L, "POWER_MOVES", false);
    self->stock->vtable->SetRecycles(self->stock, MoonGetGlobalInt(self->L, "STOCK_RECYCLES", 32767));

    // CELL_ACCEPT, DISCARD_ACCEPT, RESERVE_ACCEPT, STOCK_ACCEPT, WASTE_ACCEPT don't count
    // FOUNDATION_ACCEPT, TABLEAU_ACCEPT do count

    // TODO think about individual foundations/tableau having their own accept, rather than applying globally to all

    int accept;
    size_t index;

    // by default, accept anything (that's what 0 means)
    accept = MoonGetGlobalInt(self->L, FOUNDATION_ACCEPT, 0);
    for ( struct Pile* p = ArrayFirst(self->foundations, &index); p; p = ArrayNext(self->foundations, &index) ) {
        p->vtable->SetAccept(p, accept);
    }
    accept = MoonGetGlobalInt(self->L, TABLEAU_ACCEPT, 0);
    for ( struct Pile* p = ArrayFirst(self->tableaux, &index); p; p = ArrayNext(self->tableaux, &index) ) {
        p->vtable->SetAccept(p, accept);
    }
}

void BaizeStartGame(struct Baize *const self)
{
    unsigned crc = BaizeCRC(self);

    if (lua_getglobal(self->L, "StartGame") != LUA_TFUNCTION) {  // push Lua function name onto the stack
        fprintf(stderr, "StartGame is not a function\n");
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

static bool setupTableMethod(lua_State *L, const char *table, const char *method)
{   // TODO refactor out this duplicate
    int typ = lua_getglobal(L, table);
    if (typ != LUA_TTABLE) {
        fprintf(stderr, "ERROR: %s: %s is not a table\n", __func__, table);
        lua_pop(L, 1);  // remove table name
        return false;
    }
    typ = lua_getfield(L, -1, method);
    if (typ != LUA_TFUNCTION) {
        // fprintf(stderr, "WARNING: %s: %s.%s is not a function\n", __func__, table, method);
        lua_pop(L, 2);  // remove table and method names
        return false;
    }
    return true;
}

void BaizeTailTapped(struct Baize *const self)
{
    if (!self->tail || ArrayLen(self->tail)==0) {
        fprintf(stderr, "ERROR: %s: passed nil tail\n", __func__);
    }

    lua_State *L = self->L;
    struct Card* c0 = ArrayGet(self->tail, 0);
    struct Pile* pile = c0->owner;

    if (!setupTableMethod(L, pile->category, "Tapped")) {
        fprintf(stderr, "%s.Tapped is not a function, reverting to internal default (tail len %lu)\n", pile->category, ArrayLen(self->tail));
        pile->vtable->Tapped(pile, self->tail);
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

    if (!setupTableMethod(L, pile->category, "Tapped")) {
        fprintf(stderr, "%s.Tapped is not a function, reverting to internal default (nil tail)\n", pile->category);
        pile->vtable->Tapped(pile, NULL);
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
