/* exiface.c */

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "exiface.h"
#include "trace.h"

struct knownInterface {
    char name[32];
    struct ExecutionInterface* (*Ctor)(void);
};

static struct knownInterface knownInterfaces[3] = {
    { "Fallback", GetFallbackInterface  },  // fallback comes first
    { "Freecell", GetFreecellInterface  },
    { "Klondike", GetKlondikeInterface  },
};

// TODO baize->variantName is probably already set so retire name parameter
struct ExecutionInterface* GetInterface(struct Baize *const baize)
{
    struct ExecutionInterface *self = (void*)0;

    // try to find a Lua scripted game first, so inbuilt games can be overridden
    char fname[128];    snprintf(fname, 127, "variants/%s.lua", baize->variantName);
    if ( luaL_loadfile(baize->L, fname) || lua_pcall(baize->L, 0, 0, 0) ) {
        CSOL_ERROR("Cannot find script for game '%s', error: %s", baize->variantName, lua_tostring(baize->L, -1));
        lua_pop(baize->L, 1);

        for ( size_t i=0; i<sizeof(knownInterfaces)/sizeof(struct knownInterface); i++ ) {
            if (strcmp(knownInterfaces[i].name, baize->variantName) == 0) {
                self = knownInterfaces[i].Ctor();
                break;
            }
        }
    } else {
        CSOL_INFO("loaded '%s'", fname);
        // we found a Lua script, and it loads, so use the csol-Lua bridge interface
        self = GetMoonInterface();
    }

    if (!self) {
        CSOL_INFO("Cannot find rules for game '%s', using fallback", baize->variantName);
        self = knownInterfaces[0].Ctor();
    }

    return self;
}

// TODO useful generic game library of functions
