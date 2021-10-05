/* check.c */

#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "array.h"
#include "baize.h"
#include "check.h"
#include "moon.h"

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
        MoonPushCardAsTable(L, cNext);

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

static bool checkPair(struct Baize *const baize, struct Card *const cPrev, struct Card *const cNext, bool movable)
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

    if ( cPrev->prone || cNext->prone ) {
        fprintf(stderr, "WARNING: %s passed a face down card\n", __func__);
        return false;
    }

    char funcName[64];
    strcpy(funcName, "Check");
    strcat(funcName, cPrev->owner->category);
    if ( movable ) {
        strcat(funcName, "Movable");
    }

    int typ = lua_getglobal(L, funcName);  // push Lua function name onto the stack
    if ( typ != LUA_TFUNCTION ) {
        fprintf(stderr, "%s is not a function\n", funcName);
        lua_pop(L, 1);  // remove func from stack
    } else {
        MoonPushCardAsTable(L, cPrev);
        MoonPushCardAsTable(L, cNext);

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

static bool checkTail(struct Baize *const baize, struct Array *const tail)
{
    if ( ArrayLen(tail) == 0 ) {
        fprintf(stderr, "WARNING: %s passed empty tail\n", __func__);
        return false;
    }

    struct Card *cPrev = ArrayGet(tail, 0);

    if ( !cPrev->owner->draggable ) {
        char z[128];
        sprintf(z, "You may not move cards from a %s", cPrev->owner->category);
        BaizeSetError(baize, z);
        return false;
    }

    if ( ArrayLen(tail) == 1 ) {
        fprintf(stderr, "WARNING: %s passed tail with one card\n", __func__);
        return true;
    }

    size_t i = 1;
    while ( i < ArrayLen(tail) ) {
        struct Card *cNext = ArrayGet(tail, i);
        if ( !checkPair(baize, cPrev, cNext, true) ) {
            return false;
        }

        cPrev = cNext;
        i++;
    }
    
    return true;
}

bool CheckAccept(struct Baize *const baize, struct Pile *const dstPile, struct Card *const c)
{
    return checkAccept(baize, dstPile, c);
}

bool CheckCard(struct Baize *const baize, struct Pile *const dstPile, struct Card *const c)
{
    if ( PileEmpty(dstPile) ) {
        return checkAccept(baize, dstPile, c);
    } else {
        struct Card *cPrev = PilePeekCard(dstPile);
        return checkPair(baize, cPrev, c, false);
    }
}

bool CheckPair(struct Baize *const baize, struct Card *const cPrev, struct Card *const cNext)
{
    return checkPair(baize, cPrev, cNext, false);
}

bool CheckTail(struct Baize *const baize, struct Pile *const dstPile, struct Array *const tail)
{
    // if ( PileEmpty(dstPile) ) {
    //     if ( !checkAccept(baize, dstPile, ArrayGet(tail, 0)) ) {
    //         return false;
    //     }
    // }
    (void)dstPile;  // TODO retire
    return checkTail(baize, tail);
}
