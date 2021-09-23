/* conformant.c */

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "array.h"
#include "baize.h"
#include "conformant.h"
#include "moon.h"

static bool Conformant(lua_State *L, const char* func, struct Card *c, struct Array* tail)
{
    // fprintf(stdout, "Conformant Lua stack in  %d\n", lua_gettop(L));

    fprintf(stdout, "Conformant func %s\n", func);

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

        // one arg (card-as-a-table), one return (boolean)
        if ( lua_pcall(L, 2, 1, 0) != LUA_OK ) {
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
            lua_pop(L, 1);  // remove returned boolean from stack
        }
    }

    // fprintf(stdout, "ConformantTail Lua stack out %d\n", lua_gettop(L));

    return result;
}

bool ConformantBuild(lua_State *L, struct Pile *const pile, struct Card* c, struct Array *tail)
{
    fprintf(stderr, "ConformantBuild '%s' '%s'\n", pile->category, pile->buildfunc);

    if ( pile->buildfunc[0] == '\0' ) {
        return false;
    }
    if ( ArrayLen(tail) == 0 ) {
        fprintf(stderr, "WARNING: ConformantBuild passed empty tail\n");
        return false;
    }
    return Conformant(L, pile->buildfunc, c, tail);
}

bool ConformantDrag(lua_State *L, struct Pile *const pile, struct Array* tail)
{
    fprintf(stderr, "ConformantDrag '%s' '%s'\n", pile->category, pile->dragfunc);

    if ( pile->dragfunc[0] == '\0' ) {
        return pile == pile->owner->stock;
    }
    if ( ArrayLen(tail) == 0 ) {
        fprintf(stderr, "WARNING: ConformantDrag passed empty tail\n");
        return false;
    }

    return Conformant(L, pile->dragfunc, NULL, tail);
}
