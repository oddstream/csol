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
    strcpy(funcName, dstPile->category);
    strcat(funcName, "Accept");

    int typ = lua_getglobal(L, funcName);  // push Lua function name onto the stack
    if ( typ != LUA_TFUNCTION ) {
        fprintf(stderr, "%s is not a function\n", funcName);
        lua_pop(L, 1);  // remove func from stack
    } else {
        lua_pushlightuserdata(L, dstPile);
        MoonPushCardAsTable(L, cNext);

        // one arg (dest pile, card-as-a-table), two returns (boolean, error string)
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

static bool checkPair(struct Baize *const baize, struct Card *const cPrev, struct Card *const cNext, bool movable)
{
    lua_State *L = baize->L;
    bool result = false;
    BaizeResetError(baize);

    // cPrev is allowed to by NULL (accept card to an empty pile), but cNext isn't

    if ( !CardValid(cNext) ) {
        fprintf(stderr, "WARNING: %s passed invalid next card\n", __func__);
        return false;
    }

    if ( (cPrev && cPrev->prone) || cNext->prone ) {
        fprintf(stderr, "WARNING: %s passed a face down card\n", __func__);
        return false;
    }

    char funcName[64];
    strcpy(funcName, cPrev->owner->category);
    if ( movable ) {
        strcat(funcName, "MovePair");
    } else {
        strcat(funcName, "BuildPair");
    }

    int typ = lua_getglobal(L, funcName);  // push Lua function name onto the stack
    if ( typ != LUA_TFUNCTION ) {
        fprintf(stderr, "%s is not a function\n", funcName);
        lua_pop(L, 1);  // remove func from stack
    } else {
        MoonPushCardAsTable(L, cPrev);  // okay with NULL
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

static bool checkTailCards(struct Baize *const baize, struct Array *const tail)
{
    if ( ArrayLen(tail) == 0 ) {
        fprintf(stderr, "ERROR: %s passed empty tail\n", __func__);
        return false;
    }

    struct Card *cPrev = ArrayGet(tail, 0);

    if ( !cPrev->owner->draggable ) {
        char z[128];
        sprintf(z, "You may not move cards from a %s", cPrev->owner->category);
        BaizeSetError(baize, z);
        return false;
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
    // return true if (an empty) dstPile can accept the card
    return checkAccept(baize, dstPile, c);
}

// bool CheckCard(struct Baize *const baize, struct Pile *const dstPile, struct Card *const c)
// {
//     if ( PileEmpty(dstPile) ) {
//         return checkAccept(baize, dstPile, c);
//     } else {
//         struct Card *cPrev = PilePeekCard(dstPile);
//         return checkPair(baize, cPrev, c, false);
//     }
// }

bool CheckPair(struct Baize *const baize, struct Card *const cPrev, struct Card *const cNext)
{
    // return true if it's ok to build cNext onto cPrev
    return checkPair(baize, cPrev, cNext, false);
}

bool CheckCards(struct Baize *const baize, struct Pile *const pile)
{
    // return true if the cards in this pile are built correctly (an empty pile is built correctly)
    if ( PileEmpty(pile) ) {
        return true;
    }

    struct Card *cPrev = ArrayGet(pile->cards, 0);
    size_t i = 1;
    while ( i < ArrayLen(pile->cards) ) {
        struct Card *cNext = ArrayGet(pile->cards, i);
        if ( !checkPair(baize, cPrev, cNext, false) ) {
            return false;
        }
        cPrev = cNext;
        i++;
    }
    
    return true;
}

static bool checkTailMovable(struct Baize *const baize, struct Array *const tail)
{
    /*
        ask Lua if movement is constrained by only allowing
        1. one card at a time to be moved
        2. one card or the whole pile to be moved (eg American Toad)
        3. any amount of cards to be moved

        The number of moves that can be made if POWERMOVES is set is only
        known when the destination pile is known, which makes it a "can build"
        function, not a "can drag" function.
    */
    lua_State *L = baize->L;
    bool result = true;
    BaizeResetError(baize);

    if ( ArrayLen(tail) == 0 ) {
        fprintf(stderr, "ERROR: %s: empty tail\n", __func__);
        return false;
    }

    if ( ArrayLen(tail) == 1 ) {
        return true;    // shirley one card is always movable?
    }

    struct Card *const c0 = ArrayGet(tail, 0);

    char funcName[64];
    strcpy(funcName, c0->owner->category);
    strcat(funcName, "MoveTail");

    int typ = lua_getglobal(L, funcName);  // push Lua function name onto the stack
    if ( typ != LUA_TFUNCTION ) {
        fprintf(stderr, "%s is not a function\n", funcName);
        lua_pop(L, 1);  // remove func from stack
    } else {
        lua_pushinteger(L, ArrayLen(c0->owner->cards));    // length of originating pile
        lua_pushinteger(L, ArrayLen(tail));         // length of tail

        // two args (int len, int len), two returns (boolean, error string)
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

bool CheckDragTail(struct Baize *const baize, struct Array *const tail)
{
    // returns true if it's ok to drag this tail
    return checkTailMovable(baize, tail) && checkTailCards(baize, tail);
}
