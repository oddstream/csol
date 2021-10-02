/* moon.c */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>

#include "baize.h"
#include "array.h"
#include "pile.h"
#include "stock.h"
#include "cell.h"
#include "foundation.h"
#include "tableau.h"
#include "waste.h"
#include "moon.h"

static const struct FunctionToRegister {
    char luaFunction[32];
    lua_CFunction cFunction;
} FunctionsToRegister[] = {
    {"AddPile", MoonAddPile},
    {"DealUp", MoonDealUp},
    {"DealDown", MoonDealDown},
    {"FindPile", MoonFindPile},
    {"MovePileTo", MoonMovePileTo},
    {"PileCategory", MoonGetPileCategory},
    {"CardCount", MoonGetCardCount},
    {"CardOwner", MoonGetCardOwner},
    {"PowerMoves", MoonGetPowerMoves},
    {"SetAccept", MoonSetAccept},
    {"SetRecycles", MoonSetRecycles},
    {"PeekCard", MoonPeekCard},
    {"MoveCard", MoonMoveCard},
};

static struct Baize* getBaize(lua_State* L)
{
    int typ = lua_getglobal(L, "BAIZE");    // push light userdata on the stack
    if ( typ != LUA_TLIGHTUSERDATA ) {
        fprintf(stderr, "global BAIZE is not light userdata\n");
        lua_pop(L, 1);  // remove "BAIZE" from stack
    }
    struct Baize* baize = lua_touserdata(L, -1); // doesn't alter stack
    lua_pop(L, 1);  // pop light userdata
    if ( !BaizeValid(baize) ) {
        fprintf(stderr, "global BAIZE is not valid\n");
    }
    return baize;
}

void MoonRegisterFunctions(lua_State* L)
{
    for ( size_t i=0; i<sizeof(FunctionsToRegister) / sizeof(struct FunctionToRegister); i++ ) {
        lua_pushcfunction(L, FunctionsToRegister[i].cFunction);
        lua_setglobal(L, FunctionsToRegister[i].luaFunction);
    }
}

bool MoonGetGlobalBool(lua_State* L, const char* var, const bool def)
{
    bool result = def;
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    int typ = lua_getglobal(L, var);    // pushes value onto stack
    if ( typ == LUA_TNIL ) {
        fprintf(stderr, "%s is nil\n", var);
        result = def;
    } else if ( typ != LUA_TBOOLEAN ) {
        fprintf(stderr, "%s is not a boolean\n", var);
        result = def;
    } else {
        result = lua_toboolean(L, -1); // does not alter stack
    }
    lua_pop(L, 1); // remove boolean from stack
    fprintf(stderr, "%s=%d\n", var, result);
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    return result;
}

int MoonGetGlobalInt(lua_State* L, const char* var, const int def)
{
    int result = def;
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    int typ = lua_getglobal(L, var);    // pushes value onto stack
    if ( typ == LUA_TNIL ) {
        fprintf(stderr, "%s is nil\n", var);
        result = def;
    } else if ( typ != LUA_TNUMBER ) {
        fprintf(stderr, "%s is not a number\n", var);
        result = def;
    } else {
        int isnum;
        result = (int)lua_tointegerx(L, -1, &isnum); // does not alter stack
        if ( !isnum ) {
            fprintf(stderr, "%s cannot be converted to an integer\n", var);
            result = def;
        }
    }
    lua_pop(L, 1); // remove integer from stack
    fprintf(stderr, "%s=%d\n", var, result);
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    return result;
}

float MoonGetGlobalFloat(lua_State* L, const char* var, const float def)
{
    float result = def;
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    int typ = lua_getglobal(L, var);    // pushes value onto stack
    if ( typ == LUA_TNIL ) {
        fprintf(stderr, "%s is nil\n", var);
        result = def;
    } else if ( typ != LUA_TNUMBER ) {
        fprintf(stderr, "%s is not a number\n", var);
        result = def;
    } else {
        int isnum;
        result = (float)lua_tonumberx(L, -1, &isnum); // does not alter stack
        if ( !isnum ) {
            fprintf(stderr, "%s cannot be converted to a number\n", var);
            result = def;
        }
    }
    lua_pop(L, 1); // remove integer from stack
    fprintf(stderr, "%s=%f\n", var, result);
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    return result;
}

const char* MoonGetGlobalString(lua_State* L, const char* var, const char* def)
{
    const char* result = def;
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    int typ = lua_getglobal(L, var);    // pushes value onto stack
    if ( typ != LUA_TSTRING ) {
        fprintf(stderr, "%s is not a string\n", var);
        result = def;
    } else {
        result = lua_tostring(L, -1); // does not alter stack
        if ( !result ) {
            fprintf(stderr, "%s cannot be converted to a string\n", var);
            result = def;
        }
    }
    lua_pop(L, 1); // remove integer from stack
    fprintf(stderr, "%s=%s\n", var, result);
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    return result;
}

float MoonGetFieldNumber(lua_State* L, const char* key, const float def)
{
    // assumes table is on top of stack
    float result = def;
    int isnum;
    int  typ = lua_getfield(L, -1, key);    // pushes onto the stack the value t[k], where t is the value at the given index
    if ( typ == LUA_TNIL ) {
        fprintf(stderr, "%s is nil\n", key);
        result = def;
    } else if ( typ != LUA_TNUMBER ) {
        fprintf(stderr, "%s is not a number\n", key);
        result = def;
    } else {
        result = (float)lua_tonumberx(L, -1, &isnum);    // returns a lua_Number
        if ( !isnum ) {
            fprintf(stderr, "%s cannot be converted to a number\n", key);
            result = def;
        }
    }
    lua_pop(L, 1);  // remove number
    return result;
}

void MoonPushCard(lua_State *L, struct Card *const c)
{
    if ( c ) {
        if ( !CardValid(c) ) {
            fprintf(stderr, "WARNING: Invalid card in %s\n", __func__);
        }
        lua_createtable(L, 0, 5);       // create and push new 4-element table
        
        lua_pushinteger(L, c->id.ordinal);
        lua_setfield(L, -2, "ordinal"); // table["ordinal"] = c->ord, pops key value

        lua_pushinteger(L, c->id.suit);
        lua_setfield(L, -2, "suit");    // table["suit"] = c->suit, pops key value

        lua_pushinteger(L, c->id.suit == DIAMOND || c->id.suit == HEART ? 1 : 0);
        lua_setfield(L, -2, "color");   // table["color"] == [0|1], pops key value

        lua_pushboolean(L, c->prone);
        lua_setfield(L, -2, "prone");   // table["prone"] = c->prone, pops key value

        lua_pushlightuserdata(L, c->owner);
        lua_setfield(L, -2, "owner");   // table["owner"] = c->owner, pops key value
    } else {
        lua_pushnil(L);
    }
}

void MoonPushTail(lua_State *L, struct Array *const tail)
{
    // fprintf(stdout, "MoonPushTail Lua stack in  %d\n", lua_gettop(L));

    // build table on stack
    // outer table is an array/sequence, same length as tail
    // each inner table is a record of each card eg {ordinal=1, suit=SPADE, color=0, prone=false}
    lua_createtable(L, ArrayLen(tail), 0);  // create and push tail table

    size_t index;
    struct Card* c = ArrayFirst(tail, &index);
    while ( c ) {
        MoonPushCard(L, c);
        lua_seti(L, -2, index + 1);     // add the card-table to the underlying table
        c = ArrayNext(tail, &index);
    }

    // fprintf(stdout, "MoonPushTail Lua stack out %d\n", lua_gettop(L));
}

int MoonAddPile(lua_State* L)
{
    struct Baize* baize = getBaize(L);
    if ( !BaizeValid(baize) ) {
        return 0;
    }

    const char* category = lua_tostring(L, 1); // doesn't alter stack
    // the slot numbers in Lua start from 1, and start from 0 in C
    float x = lua_tonumber(L, 2) - 1; // doesn't alter stack
    float y = lua_tonumber(L, 3) - 1; // doesn't alter stack
    enum FanType fan = lua_tointeger(L, 4); // doesn't alter stack
    const char* buildfunc = lua_tostring(L, 5);
    const char* dragfunc = lua_tostring(L, 6);

    // fprintf(stderr, "PileNew(%s,%f,%f,%d)\n", category, x, y, fan);

    struct Pile* p = NULL;
    if ( strcmp(category, "Stock") == 0 ) {
        p = (struct Pile*)StockNew((Vector2){x, y}, fan, buildfunc, dragfunc);
    } else if  ( strcmp(category, "Cell") == 0 ) {
        p = (struct Pile*)CellNew((Vector2){x, y}, fan, buildfunc, dragfunc);
    } else if ( strcmp(category, "Foundation") == 0 ) {
        p = (struct Pile*)FoundationNew((Vector2){x, y}, fan, buildfunc, dragfunc);
    } else if  ( strcmp(category, "Tableau") == 0 ) {
        p = (struct Pile*)TableauNew((Vector2){x, y}, fan, buildfunc, dragfunc);
    } else if  ( strcmp(category, "Waste") == 0 ) {
        p = (struct Pile*)WasteNew((Vector2){x, y}, fan, buildfunc, dragfunc);
    } else {
        fprintf(stderr, "Unknown pile category %s\n", category);
    }
    if ( PileValid(p) ) {
        p->owner = baize;
        ArrayPush(baize->piles, p);
        lua_pushlightuserdata(L, p);
        return 1;
    } else {
        return 0;
    }
}

int MoonDealUp(lua_State* L)
{
    struct Baize* baize = getBaize(L);
    if ( !BaizeValid(baize) ) {
        return 0;
    }

    struct Pile* p = lua_touserdata(L, 1); // get argument
    if ( !PileValid(p) ) {
        fprintf(stderr, "destination pile is not valid");
        return 0;
    }
    
    int n = lua_tointeger(L, 2);
    while ( n-- ) {
        struct Card* c = PilePopCard(baize->stock);
        if ( c ) {
            CardFlipUp(c);
            PilePushCard(p, c);
        } else {
            fprintf(stderr, "cannot pop card from Stock\n");
        }
    }

    return 0;
}

int MoonDealDown(lua_State* L)
{
    struct Baize* baize = getBaize(L);
    if ( !BaizeValid(baize) ) {
        return 0;
    }

    struct Pile* p = lua_touserdata(L, 1); // get argument
    if ( !PileValid(p) ) {
        fprintf(stderr, "destination pile is not valid");
        return 0;
    }
    
    int n = lua_tointeger(L, 2);
    while ( n-- ) {
        struct Card* c = PilePopCard(baize->stock);
        if ( c ) {
            CardFlipDown(c);
            PilePushCard(p, c);
        } else {
            fprintf(stderr, "cannot pop card from Stock\n");
        }
    }

    return 0;
}

int MoonFindPile(lua_State* L)
{
    struct Baize* baize = getBaize(L);
    if ( !BaizeValid(baize) ) {
        return 0;
    }

    const char* category = lua_tostring(L, 1); // doesn't alter stack
    int n = lua_tointeger(L, 2); // doesn't alter stack

    struct Pile *p = BaizeFindPile(baize, category, n);
    if ( PileValid(p) ) {
        lua_pushlightuserdata(L, p);
        return 1;
    }

    return 0;
}

int MoonMovePileTo(lua_State* L)
{
    struct Pile* p = lua_touserdata(L, 1);
    float x = lua_tonumber(L, 2) - 1;
    float y = lua_tonumber(L, 3) - 1;

    if ( PileValid(p) ) {
        p->slot = (Vector2){.x=x, .y=y};
        p->pos = PileCalculatePosFromSlot(p);
        PileRepushAllCards(p);
        // float dx = newPos.x - oldPos.x;
        // float dy = newPos.y - oldPos.y;
        // size_t index;
        // for ( struct Card* c = ArrayFirst(p->cards, &index); c; c = ArrayNext(p->cards, &index) ) {
        //     Vector2 oldCardPos = CardGetBaizePos(c);
        //     Vector2 newCardPos = (Vector2){.x = oldCardPos.x + dx, .y = oldCardPos.y + dy};
        //     CardSetPos(c, newCardPos);
        // }
    }

    return 0;
}

int MoonGetPileCategory(lua_State *L)
{
    struct Pile* p = lua_touserdata(L, 1);
    if ( !PileValid(p) ) {
        fprintf(stderr, "WARNING: PileCategory: invalid pile\n");
        lua_pushnil(L);
    } else {
        lua_pushstring(L, p->category);
    }
    return 1;
}

int MoonGetCardCount(lua_State *L)
{
    struct Pile* p = lua_touserdata(L, 1);
    if ( PileValid(p) ) {
        lua_pushinteger(L, ArrayLen(p->cards));
    } else {
        lua_pushinteger(L, 0);
    }
    return 1;
}

int MoonGetCardOwner(lua_State *L)
{
    struct Card* c = lua_touserdata(L, 1);
    if ( !CardValid(c) ) {
        fprintf(stderr, "WARNING: CardOwner: invalid card\n");
        return 0;
    }
    lua_pushlightuserdata(L, c->owner);
    return 1;
}

int MoonGetPowerMoves(lua_State *L)
{
    struct Baize *baize = lua_touserdata(L, 1);
    struct Pile *dstPile = lua_touserdata(L, 2);

    if ( !BaizeValid(baize) ) {
        fprintf(stderr, "WARNING: PowerMoves: invalid baize\n");
        return 0;
    }
    if ( dstPile != NULL && !PileValid(dstPile) ) {
        fprintf(stderr, "WARNING: PowerMoves: invalid pile\n");
        return 0;
    }

    double emptyCells = 0.0;
    double emptyCols = 0.0;
    size_t index;
    for ( struct Pile *p=ArrayFirst(baize->piles, &index); p; p=ArrayNext(baize->piles, &index) ) {
        if ( ArrayLen(p->cards) == 0 ) {
            if ( strcmp(p->category, "Cell") == 0 ) {
                emptyCells++;
            } else if ( strcmp(p->category, "Tableau") == 0 ) {
                // 'If you are moving into an empty column, then the column you are moving into does not count as empty column.'
                struct Tableau *t = (struct Tableau*)p;
                if ( t->accept == 0 && p != dstPile ) {
                    emptyCols++;
                }
            }
        }
    }

    double n = (1.0 + emptyCells) * pow(2.0, emptyCols);
    lua_pushinteger(L, (int)n);
    return 1;
}

int MoonSetAccept(lua_State *L)
{
    struct Pile* p = lua_touserdata(L, 1);
    enum CardOrdinal ord = lua_tointeger(L, 2);

    if ( PileValid(p) ) {
        p->vtable->SetAccept(p, ord);
    }

    return 0;
}

int MoonSetRecycles(lua_State *L)
{
    struct Pile* p = lua_touserdata(L, 1);
    int r = lua_tointeger(L, 2);

    if ( PileValid(p) ) {
        p->vtable->SetRecycles(p, r);
    }

    return 0;
}

int MoonPeekCard(lua_State *L)
{
    struct Pile *p = lua_touserdata(L, 1);

    if ( !PileValid(p) ) {
        fprintf(stderr, "MoonPeekCard pile not valid\n");
        lua_pushnil(L);
        return 1;
    }

    MoonPushCard(L, PilePeekCard(p));    // may be NULL if pile empty
    return 1;
}

int MoonMoveCard(lua_State *L)
{
    struct Pile *src = lua_touserdata(L, 1);
    struct Pile* dst = lua_touserdata(L, 2);

    bool cardsMoved = false;

    if ( !PileValid(src) ) {
        fprintf(stderr, "MoonMoveCards source pile not valid\n");
        lua_pushboolean(L, false);
        return 1;
    }

    if ( !PileValid(dst) ) {
        fprintf(stderr, "MoonMoveCards destination pile not valid\n");
        lua_pushboolean(L, false);
        return 1;
    }

    struct Card *c = PilePeekCard(src);
    if ( c ) {
        if ( PileMoveCards(dst, c) ) {
            cardsMoved = true;
        }
    }

    lua_pushboolean(L, cardsMoved);
    return 1;
}
