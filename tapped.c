/* tapped.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <raylib.h>
#include <lua.h>

#include "baize.h"
#include "moon.h"

bool BaizeCardTapped(struct Baize *const self, struct Card* c)
{
    lua_State *L = self->L;
    bool cardsMoved = true;
    self->errorString[0] = '\0';

    int typ = lua_getglobal(L, "CardTapped");  // push function name onto the stack
    if ( typ != LUA_TFUNCTION ) {
        fprintf(stderr, "CardTapped is not a function\n");
        lua_pop(L, 1);  // remove function name
        return false;
    }

    // push one arg, the card (as a table)
    MoonPushCard(L, c);

    // one arg (card-as-a-table), two returns (boolean cards moved, error string)
    if ( lua_pcall(L, 1, 2, 0) != LUA_OK ) {
        fprintf(stderr, "error running Lua function: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    } else {
        // fprintf(stderr, "%s called ok\n", func);
        if ( lua_isboolean(L, 1) ) {
            cardsMoved = lua_toboolean(L, 1);
        } else {
            fprintf(stderr, "WARNING: expecting boolean return from CardTapped\n");
            cardsMoved = false;
        }
        if ( lua_isnil(L, 2) ) {
            self->errorString[0] = '\0';
        } else if ( lua_isstring(L, 2) ) {
            const char *str = lua_tostring(L, 2);
            if ( str ) {
                strncpy(self->errorString, str, sizeof(self->errorString) - 1);
            }
        } else {
            fprintf(stderr, "WARNING: expecting string return from CardTapped\n");
            cardsMoved = false;
        }
        lua_pop(L, 2);  // remove returned boolean, string from stack
    }

    return cardsMoved;
}
