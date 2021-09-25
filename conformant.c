/* conformant.c */

#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "array.h"
#include "baize.h"
#include "conformant.h"
#include "moon.h"

static bool Conformant(struct Baize *const baize, const char* func, struct Card *c, struct Array* tail)
{
    // fprintf(stdout, "Conformant Lua stack in  %d\n", lua_gettop(L));

    fprintf(stdout, "Conformant func %s\n", func);

    lua_State *L = baize->L;
    BaizeResetError(baize);

    if ( func == NULL || func[0] == '\0' ) return false;

    bool result = true;

    int typ = lua_getglobal(L, func);  // push Lua function name onto the stack
    if ( typ != LUA_TFUNCTION ) {
        fprintf(stderr, "%s is not a function\n", func);
        lua_pop(L, 1);  // remove func from stack
    } else {
        // card we are building on as first arg (may be nil/NULL)
        MoonPushCard(L, c);
        // build table on stack as second arg
        MoonPushTail(L, tail);

        // one arg (card-as-a-table), two returns (boolean, error string)
        if ( lua_pcall(L, 2, 2, 0) != LUA_OK ) {
            fprintf(stderr, "error running Lua function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        } else {
            // fprintf(stderr, "%s called ok\n", func);
            if ( lua_isboolean(L, 1) ) {
                result = lua_toboolean(L, 1);
            } else {
                fprintf(stderr, "WARNING: expecting boolean return from %s\n", func);
                result = false;
            }
            if ( lua_isnil(L, 2) ) {
                ;
            } else if ( lua_isstring(L, 2) ) {
                BaizeSetError(baize, lua_tostring(L, 2));
            } else {
                fprintf(stderr, "WARNING: expecting string ir nil return from %s\n", func);
            }
            lua_pop(L, 2);  // remove returned boolean, string from stack
        }
    }

    // fprintf(stdout, "ConformantTail Lua stack out %d\n", lua_gettop(L));

    return result;
}

bool ConformantBuild(struct Baize *const baize, struct Pile *const pile, struct Card* c, struct Array *tail)
{
    fprintf(stderr, "ConformantBuild '%s' '%s'\n", pile->category, pile->buildfunc);

    if ( pile->buildfunc[0] == '\0' ) {
        return false;
    }
    if ( ArrayLen(tail) == 0 ) {
        fprintf(stderr, "WARNING: ConformantBuild passed empty tail\n");
        return false;
    }
    return Conformant(baize, pile->buildfunc, c, tail);
}

bool ConformantDrag(struct Baize *const baize, struct Pile *const pile, struct Array* tail)
{
    fprintf(stderr, "ConformantDrag '%s' '%s'\n", pile->category, pile->dragfunc);

    if ( pile->dragfunc[0] == '\0' ) {
        return pile == pile->owner->stock;
    }
    if ( ArrayLen(tail) == 0 ) {
        fprintf(stderr, "WARNING: ConformantDrag passed empty tail\n");
        return false;
    }

    return Conformant(baize, pile->dragfunc, NULL, tail);
}