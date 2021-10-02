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

bool Conformant(struct Baize *const baize, struct Pile* pileDst, const char* func, struct Array* tail)
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
        // pile we are building on as first arg (may be nil/NULL)
        if ( pileDst ) {
            // pushing a NULL as lightuserdata creates some weird object that doesn't appear in Lua as nil
            lua_pushlightuserdata(L, pileDst);
        } else {
            lua_pushnil(L);
        }
        // build table on stack as second arg
        MoonPushTail(L, tail);

        // two args (destination pile, tail of card-as-a-table), two returns (boolean, error string)
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
                fprintf(stderr, "WARNING: expecting string or nil return from %s\n", func);
            }
            lua_pop(L, 2);  // remove returned boolean, string from stack
        }
    }

    // fprintf(stdout, "ConformantTail Lua stack out %d\n", lua_gettop(L));

    return result;
}

bool ConformantBuild(struct Baize *const baize, struct Pile *const pileDst, struct Array *tail)
{
    fprintf(stderr, "ConformantBuild '%s' '%s'\n", pileDst->category, pileDst->buildfunc);

    if ( pileDst->buildfunc[0] == '\0' ) {
        return false;
    }
    if ( ArrayLen(tail) == 0 ) {
        fprintf(stderr, "WARNING: ConformantBuild passed empty tail\n");
        return false;
    }
    return Conformant(baize, pileDst, pileDst->buildfunc, tail);
}

bool ConformantDrag(struct Baize *const baize, struct Array* tail)
{
    if ( ArrayLen(tail) == 0 ) {
        fprintf(stderr, "WARNING: ConformantDrag passed empty tail\n");
        return false;
    }

    struct Card * c0 = ArrayGet(tail, 0);
    struct Pile *pile = c0->owner;

    if ( pile->dragfunc[0] == '\0' ) {
        return pile == pile->owner->stock && ArrayLen(tail) == 1;
    }

    return Conformant(baize, NULL, pile->dragfunc, tail);
}

static bool checkAccept(struct Baize *const baize, struct Pile *const dstPile, struct Card *const cNext)
{
    lua_State *L = baize->L;
    bool result = false;
    BaizeResetError(baize);

    if ( !CardValid(cNext) ) {
        fprintf(stderr, "WARNING: %s passed invalid next card\n", __func__);
        return false;
    }

    char funcName[64];
    strcpy(funcName, "Check");
    strcat(funcName, dstPile->category);
    strcat(funcName, "Accept");

    int typ = lua_getglobal(L, funcName);  // push Lua function name onto the stack
    if ( typ != LUA_TFUNCTION ) {
        fprintf(stderr, "%s is not a function\n", funcName);
        lua_pop(L, 1);  // remove func from stack
    } else {
        MoonPushCard(L, cNext);

        // one arg (card-as-a-table), two returns (boolean, error string)
        if ( lua_pcall(L, 1, 2, 0) != LUA_OK ) {
            fprintf(stderr, "error running Lua function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        } else {
            // fprintf(stderr, "%s called ok\n", func);
            if ( lua_isboolean(L, 1) ) {
                result = lua_toboolean(L, 1);
            } else {
                fprintf(stderr, "WARNING: expecting boolean return from %s\n", funcName);
                result = false;
            }
            if ( lua_isnil(L, 2) ) {
                ;
            } else if ( lua_isstring(L, 2) ) {
                BaizeSetError(baize, lua_tostring(L, 2));
            } else {
                fprintf(stderr, "WARNING: expecting string or nil return from %s\n", funcName);
            }
            lua_pop(L, 2);  // remove returned boolean, string from stack
        }
    }

    return result;
}

static bool checkPair(struct Baize *const baize, const char *func, struct Card *const cPrev, struct Card *const cNext)
{
    lua_State *L = baize->L;
    bool result = false;
    BaizeResetError(baize);

    if ( !CardValid(cPrev) ) {
        fprintf(stderr, "WARNING: %s passed invalid prev card\n", __func__);
        return false;
    }

    if ( !CardValid(cNext) ) {
        fprintf(stderr, "WARNING: %s passed invalid next card\n", __func__);
        return false;
    }

    char funcName[64];
    strcpy(funcName, "Check");
    strcat(funcName, cPrev->owner->category);
    strcat(funcName, func);

    int typ = lua_getglobal(L, funcName);  // push Lua function name onto the stack
    if ( typ != LUA_TFUNCTION ) {
        fprintf(stderr, "%s is not a function\n", funcName);
        lua_pop(L, 1);  // remove func from stack
    } else {
        MoonPushCard(L, cPrev);
        MoonPushCard(L, cNext);

        // two args (card-as-a-table x 2), two returns (boolean, error string)
        if ( lua_pcall(L, 2, 2, 0) != LUA_OK ) {
            fprintf(stderr, "error running Lua function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        } else {
            // fprintf(stderr, "%s called ok\n", func);
            if ( lua_isboolean(L, 1) ) {
                result = lua_toboolean(L, 1);
            } else {
                fprintf(stderr, "WARNING: expecting boolean return from %s\n", funcName);
                result = false;
            }
            if ( lua_isnil(L, 2) ) {
                ;
            } else if ( lua_isstring(L, 2) ) {
                BaizeSetError(baize, lua_tostring(L, 2));
            } else {
                fprintf(stderr, "WARNING: expecting string or nil return from %s\n", funcName);
            }
            lua_pop(L, 2);  // remove returned boolean, string from stack
        }
    }

    return result;
}

static bool checkTail(struct Baize *const baize, const char* func, struct Array *const tail)
{
    if ( ArrayLen(tail) == 0 ) {
        fprintf(stderr, "WARNING: %s passed empty tail\n", __func__);
        return false;
    }

    if ( ArrayLen(tail) == 1 ) {
        fprintf(stderr, "WARNING: %s passed tail with one card\n", __func__);
        return false;
    }

    struct Card *cPrev = ArrayGet(tail, 0);
    size_t i = 1;
    while ( i < ArrayLen(tail) ) {
        struct Card *cNext = ArrayGet(tail, i);
        if ( !checkPair(baize, func, cPrev, cNext) ) {
            return false;
        }

        cPrev = cNext;
        i++;
    }
    
    return true;
}

bool CheckBuildCard(struct Baize *const baize, struct Pile *const dstPile, struct Card *const c)
{
    if ( PileEmpty(dstPile) == 0 ) {
        return checkAccept(baize, dstPile, c);
    } else {
        struct Card *cPrev = PilePeekCard(dstPile);
        return checkPair(baize, "Build", cPrev, c);
    }
}

bool CheckBuildPair(struct Baize *const baize, struct Card *const cPrev, struct Card *const cNext)
{
    return checkPair(baize, "Build", cPrev, cNext);
}

bool CheckBuildTail(struct Baize *const baize, struct Array *const tail)
{
    return checkTail(baize, "Build", tail);
}

// there isn't a CheckDragCard

// there may not need to be a CheckDragPair
bool CheckDragPair(struct Baize *const baize, struct Card *const cPrev, struct Card *const cNext)
{
    return checkPair(baize, "Drag", cPrev, cNext);
}

bool CheckDragTail(struct Baize *const baize, struct Array *const tail)
{
    if ( ArrayLen(tail) == 1 ) {
        return true;
    }
    return checkTail(baize, "Drag", tail);
}
