/* game.c */

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "game.h"
#include "trace.h"

struct KnownGame {
    char name[32];
    struct Game* (*Ctor)(struct Baize*);
};

static struct KnownGame knownGames[2] = {
    { "Klondike", KlondikeCtor },
    { "FreeCell", FreecellCtor },
};

// TODO baize->variantName is probably already set so retire name parameter
struct Game* GameCtor(struct Baize *const baize, const char* name)
{
    struct Game *self = (void*)0;
    for ( size_t i=0; i<sizeof(knownGames)/sizeof(struct KnownGame); i++ ) {
        if (strcmp(knownGames[i].name, name) == 0) {
            self = knownGames[i].Ctor(baize);
            break;
        }
    }
    if (!self) {
        CSOL_INFO("No built-in game '%s', trying Lua", name);
        { // scope for fname
            char fname[128];    snprintf(fname, 127, "variants/%s.lua", name);
            if ( luaL_loadfile(baize->L, fname) || lua_pcall(baize->L, 0, 0, 0) ) {
                CSOL_ERROR("%s", lua_tostring(baize->L, -1));
                lua_pop(baize->L, 1);
            } else {
                self = MoonGameCtor(baize);
            }
        }
    }

    return self;
}

void GameDtor(struct Game* game)
{
    if (game) {
        free(game);
    }
}

// TODO useful generic game library of functions
