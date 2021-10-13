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
    int typ = lua_getglobal(self->L, "StartGame");  // push function name onto the stack
    if ( typ != LUA_TFUNCTION ) {
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

bool BaizeCardTapped(struct Baize *const self, struct Card *const c)
{
    // {
    //     char z[64];
    //     CardToString(c, z);
    //     fprintf(stdout, "Card %s [%lu] tapped\n", z, (c - self->cardLibrary));
    //     fprintf(stdout, "sizeof(Card) == %lu\n", sizeof(struct Card));
    // }
    lua_State *L = self->L;

    int typ = lua_getglobal(L, "CardTapped");  // push function name onto the stack
    if ( typ != LUA_TFUNCTION ) {
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

bool BaizePileTapped(struct Baize *const self, struct Pile *const p)
{
    lua_State *L = self->L;

    int typ = lua_getglobal(L, "PileTapped");  // push function name onto the stack
    if ( typ != LUA_TFUNCTION ) {
        fprintf(stderr, "PileTapped is not a function\n");
        lua_pop(L, 1);  // remove function name
        return false;
    }

    unsigned int crc = BaizeCRC(self);

    // push one arg, the pile
    lua_pushlightuserdata(L, p);
    // one arg (pile), one return (error string or nil)
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
            fprintf(stderr, "WARNING: %s: expecting string or nil return from PileTapped\n", __func__);
        }
        lua_pop(L, 1);  // remove returned string from stack
    }

    return crc != BaizeCRC(self);
}
