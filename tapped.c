/* tapped.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <raylib.h>
#include <lua.h>

#include "baize.h"
#include "moon.h"

void BaizeStartGame(struct Baize *const self)
{
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
}

#if 0
bool BaizeCardTapped(struct Baize *const self, struct Card *const c)
{
    // {
    //     char z[64];
    //     CardToString(c, z);
    //     fprintf(stdout, "Card %s [%lu] tapped\n", z, (c - self->cardLibrary));
    //     fprintf(stdout, "sizeof(Card) == %lu\n", sizeof(struct Card));
    // }
    lua_State *L = self->L;

    if (lua_getglobal(L, "CardTapped") != LUA_TFUNCTION) {  // push Lua function name onto the stack
        fprintf(stderr, "CardTapped is not a function\n");
        lua_pop(L, 1);  // remove function name
        return false;
    }

    unsigned int crc = BaizeCRC(self);

    // push one arg, the card
    lua_pushlightuserdata(L, c);

    // one arg (card), one return (error string or nil)
    if ( lua_pcall(L, 1, 1, 0) != LUA_OK ) {
        fprintf(stderr, "ERROR: %s: running Lua function: %s\n", __func__, lua_tostring(self->L, -1));
        lua_pop(L, 1);
    } else {
        // fprintf(stderr, "%s called ok\n", func);
        if ( lua_isnil(L, 1) ) {
            ;
        } else if ( lua_isstring(L, 1) ) {
            const char *str = lua_tostring(L, 1);
            if ( str ) {
                BaizeSetError(self, str);
            }
        } else {
            fprintf(stderr, "WARNING: expecting string or nil return from CardTapped\n");
        }
        lua_pop(L, 1);  // remove returned boolean, string from stack
    }

    return crc != BaizeCRC(self);
}
#endif

bool BaizeTailTapped(struct Baize *const self)
{
    if (!self->tail || ArrayLen(self->tail)==0) {
        fprintf(stderr, "ERROR: %s: passed nil tail\n", __func__);
        return false;
    }

    lua_State *L = self->L;
    struct Card* c0 = ArrayGet(self->tail, 0);
    struct Pile* pile = c0->owner;
    char funcName[64]; sprintf(funcName, "Tapped_%s", pile->category);

    if (lua_getglobal(L, funcName) != LUA_TFUNCTION) {  // push Lua function name onto the stack
        fprintf(stderr, "%s is not a function, reverting to internal default (tail len %lu)\n", funcName, ArrayLen(self->tail));
        lua_pop(L, 1);  // remove function name
        return pile->vtable->Tapped(pile, self->tail);
    }

    unsigned int crc = BaizeCRC(self);

    // push one arg, the tail
    lua_pushlightuserdata(L, self->tail);

    // one arg (tail), one return (error string or nil)
    if ( lua_pcall(L, 1, 1, 0) != LUA_OK ) {
        fprintf(stderr, "ERROR: %s: running Lua function: %s\n", __func__, lua_tostring(self->L, -1));
        lua_pop(L, 1);
    } else {
        // fprintf(stderr, "%s called ok\n", func);
        if ( lua_isnil(L, 1) ) {
            ;
        } else if ( lua_isstring(L, 1) ) {
            const char *str = lua_tostring(L, 1);
            if ( str ) {
                BaizeSetError(self, str);
            }
        } else {
            fprintf(stderr, "WARNING: expecting string or nil return from %s\n", funcName);
        }
        lua_pop(L, 1);  // remove returned boolean, string from stack
    }

    return crc != BaizeCRC(self);
}

bool BaizePileTapped(struct Baize *const self, struct Pile *const pile)
{
    lua_State *L = self->L;
    char funcName[64]; sprintf(funcName, "Tapped_%s", pile->category);

    if (lua_getglobal(L, funcName) != LUA_TFUNCTION) {  // push Lua function name onto the stack
        fprintf(stderr, "%s is not a function, reverting to internal default (nil tail)\n", funcName);
        lua_pop(L, 1);  // remove function name
        return pile->vtable->Tapped(pile, NULL);
    }

    unsigned int crc = BaizeCRC(self);

    // push one arg, the (non existant) tail
    lua_pushnil(L);
    // one arg (nil tail), one return (error string or nil)
    if ( lua_pcall(L, 1, 1, 0) != LUA_OK ) {
        fprintf(stderr, "ERROR: %s: error running Lua function: %s\n", __func__, lua_tostring(L, -1));
        lua_pop(L, 1);
    } else {
        // fprintf(stderr, "%s called ok\n", func);
        if ( lua_isnil(L, 1) ) {
            ;
        } else if ( lua_isstring(L, 1) ) {
            const char *str = lua_tostring(L, 1);
            if ( str ) {
                BaizeSetError(self, str);
            }
        } else {
            fprintf(stderr, "ERROR: %s: expecting string or nil return from %s\n", __func__, funcName);
        }
        lua_pop(L, 1);  // remove returned string from stack
    }

    return crc != BaizeCRC(self);
}
