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
#include "card.h"
#include "stock.h"
#include "cell.h"
#include "discard.h"
#include "foundation.h"
#include "reserve.h"
#include "tableau.h"
#include "waste.h"
#include "moon.h"

static const struct FunctionToRegister {
    char luaFunction[32];
    lua_CFunction cFunction;
} FunctionsToRegister[] = {
    {"AddPile", MoonAddPile},
    {"FindPile", MoonFindPile},
    {"PileMoveTo", MoonPileMoveTo},
    {"PileType", MoonPileType},
    {"PileCardCount", MoonPileCardCount},
    {"CardOwner", MoonCardOwner},
    {"SetCardProne", MoonSetCardProne},
    {"SetPileAccept", MoonSetPileAccept},
    {"SetPileRecycles", MoonSetPileRecycles},
    {"PilePeekCard", MoonPilePeekCard},
    {"PileDemoteCards", MoonPileDemoteCards},
    {"PilePromoteCards", MoonPilePromoteCards},
    {"MoveCard", MoonMoveCard},
    {"MoveAllCards", MoonMoveAllCards},
    {"CardToTable", MoonCardToTable},
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

float MoonGetFieldFloat(lua_State* L, const char* key, const float def)
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

void MoonPushCardAsTable(lua_State *L, struct Card *const c)
{
    if ( c ) {
        if ( !CardValid(c) ) {
            fprintf(stderr, "WARNING: Invalid card in %s\n", __func__);
        }
        lua_createtable(L, 0, 7);       // create and push new 7-element table

        lua_pushinteger(L, (int)c->pos.x);
        lua_setfield(L, -2, "x"); // table["x"] = c->pos.x, pops key value

        lua_pushinteger(L, (int)c->pos.y);
        lua_setfield(L, -2, "y"); // table["y"] = c->pos.y, pops key value

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

void MoonPushTailAsTable(lua_State *L, struct Array *const tail)
{
    // fprintf(stdout, "MoonPushTail Lua stack in  %d\n", lua_gettop(L));

    // build table on stack
    // outer table is an array/sequence, same length as tail
    // each inner table is a record of each card eg {ordinal=1, suit=SPADE, color=0, prone=false}
    lua_createtable(L, ArrayLen(tail), 0);  // create and push tail table

    size_t index;
    struct Card* c = ArrayFirst(tail, &index);
    while ( c ) {
        MoonPushCardAsTable(L, c);
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

    // fprintf(stderr, "PileNew(%s,%f,%f,%d)\n", category, x, y, fan);

    struct Pile* p = NULL;
    if ( strcmp(category, "Stock") == 0 ) {
        p = (struct Pile*)StockNew((Vector2){x, y}, fan);
    } else if ( strcmp(category, "Cell") == 0 ) {
        p = (struct Pile*)CellNew((Vector2){x, y}, fan);
    } else if ( strcmp(category, "Discard") == 0 ) {
        p = (struct Pile*)DiscardNew((Vector2){x, y}, fan);
    } else if ( strcmp(category, "Foundation") == 0 ) {
        p = (struct Pile*)FoundationNew((Vector2){x, y}, fan);
    } else if ( strcmp(category, "Reserve") == 0 ) {
        p = (struct Pile*)ReserveNew((Vector2){x, y}, fan);
    } else if ( strcmp(category, "Tableau") == 0 ) {
        p = (struct Pile*)TableauNew((Vector2){x, y}, fan);
    } else if ( strcmp(category, "Waste") == 0 ) {
        p = (struct Pile*)WasteNew((Vector2){x, y}, fan);
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

int MoonPileMoveTo(lua_State* L)
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

int MoonPileType(lua_State *L)
{
    struct Pile* p = lua_touserdata(L, 1);
    if ( !PileValid(p) ) {
        fprintf(stderr, "WARNING: %s: invalid pile\n", __func__);
        lua_pushnil(L);
    } else {
        lua_pushstring(L, p->category);
    }
    return 1;
}

int MoonPileCardCount(lua_State *L)
{
    struct Pile* p = lua_touserdata(L, 1);
    if ( PileValid(p) ) {
        lua_pushinteger(L, ArrayLen(p->cards));
    } else {
        lua_pushinteger(L, 0);
    }
    return 1;
}

int MoonCardOwner(lua_State *L)
{
    struct Card* c = lua_touserdata(L, 1);
    if ( !CardValid(c) ) {
        fprintf(stderr, "WARNING: %s: invalid card\n", __func__);
        return 0;
    }
    lua_pushlightuserdata(L, c->owner);
    return 1;
}

int MoonSetCardProne(lua_State *L)
{
    struct Card* c = lua_touserdata(L, 1);
    if ( !CardValid(c) ) {
        fprintf(stderr, "WARNING: %s: invalid card\n", __func__);
        return 0;
    }
    bool prone = lua_toboolean(L, 2);
    if ( prone ) {
        CardFlipDown(c);
    } else {
        CardFlipUp(c);
    }
    return 0;
}

int MoonSetPileAccept(lua_State *L)
{
    struct Pile* p = lua_touserdata(L, 1);
    enum CardOrdinal ord = lua_tointeger(L, 2);

    if ( PileValid(p) ) {
        p->vtable->SetAccept(p, ord);
    }

    return 0;
}

int MoonSetPileRecycles(lua_State *L)
{
    struct Pile* p = lua_touserdata(L, 1);
    int r = lua_tointeger(L, 2);

    if ( PileValid(p) ) {
        p->vtable->SetRecycles(p, r);
    }

    return 0;
}

int MoonPilePeekCard(lua_State *L)
{
    struct Pile *p = lua_touserdata(L, 1);

    if ( !PileValid(p) ) {
        fprintf(stderr, "ERROR: %s: pile not valid\n", __func__);
        return 0;
    }

    lua_pushlightuserdata(L, PilePeekCard(p));
    return 1;
}

int MoonPileDemoteCards(lua_State *L)
{
    struct Pile *p = lua_touserdata(L, 1);
    if ( !PileValid(p) ) {
        fprintf(stderr, "ERROR: %s: pile not valid\n", __func__);
        return 0;
    }
    enum CardOrdinal ord = lua_tointeger(L, 2);
    if ( ord < 1 || ord > 13 ) {
        fprintf(stderr, "ERROR: %s: ordinal not valid\n", __func__);
        return 0;
    }

    if (PileEmpty(p)) {
        return 0;
    }

    struct Array *tmp = ArrayClone(p->cards);
    if (tmp) {
        ArrayReset(p->cards);
        size_t index;
        for ( struct Card *c=ArrayFirst(tmp, &index); c; c=ArrayNext(tmp, &index) ) {
            if (c->id.ordinal == ord) {
                ArrayPush(p->cards, c);
            }
        }
        for ( struct Card *c=ArrayFirst(tmp, &index); c; c=ArrayNext(tmp, &index) ) {
            if (c->id.ordinal != ord) {
                ArrayPush(p->cards, c);
            }
        }
        ArrayFree(tmp);
    }
    return 0;
}

int MoonPilePromoteCards(lua_State *L)
{
    struct Pile *p = lua_touserdata(L, 1);
    if ( !PileValid(p) ) {
        fprintf(stderr, "ERROR: %s: pile not valid\n", __func__);
        return 0;
    }
    enum CardOrdinal ord = lua_tointeger(L, 2);
    if ( ord < 1 || ord > 13 ) {
        fprintf(stderr, "ERROR: %s: ordinal not valid\n", __func__);
        return 0;
    }

    if (PileEmpty(p)) {
        return 0;
    }

    struct Array *tmp = ArrayClone(p->cards);
    if (tmp) {
        ArrayReset(p->cards);
        size_t index;
        for ( struct Card *c=ArrayFirst(tmp, &index); c; c=ArrayNext(tmp, &index) ) {
            if (c->id.ordinal != ord) {
                ArrayPush(p->cards, c);
            }
        }
        for ( struct Card *c=ArrayFirst(tmp, &index); c; c=ArrayNext(tmp, &index) ) {
            if (c->id.ordinal == ord) {
                ArrayPush(p->cards, c);
            }
        }
        ArrayFree(tmp);
    }
    return 0;
}

int MoonMoveCard(lua_State *L)
{
    struct Pile *src = lua_touserdata(L, 1);
    struct Pile* dst = lua_touserdata(L, 2);

    if ( !PileValid(src) ) {
        fprintf(stderr, "WARNING: %s source pile not valid\n", __func__);
        return 0;
    }

    if ( !PileValid(dst) ) {
        fprintf(stderr, "WARNING: %s destination pile not valid\n", __func__);
        return 0;
    }

    struct Card *c = NULL;

    if (lua_gettop(L) == 4) {
        enum CardOrdinal ord = lua_tointeger(L, 3);
        enum CardSuit suit = lua_tointeger(L, 4);
        struct CardAndIndex ci = PileFindCard(src, ord, suit);
        if (ci.card) {
            ArrayDelete(src->cards, ci.index, NULL);
            PileRepushAllCards(src);
            c = ci.card;
            PilePushCard(dst, c);
            CardFlipUp(c);
        }
    } else {
        if ( PileMoveCard(dst, src) ) {
            c = PilePeekCard(dst);
        }
    }

    if (c) {
        lua_pushlightuserdata(L, c);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

int MoonMoveAllCards(lua_State *L)
{
    struct Pile *src = lua_touserdata(L, 1);
    struct Pile* dst = lua_touserdata(L, 2);

    if ( !PileValid(src) ) {
        fprintf(stderr, "ERROR: %s source pile not valid\n", __func__);
        return 0;
    }

    if ( !PileValid(dst) ) {
        fprintf(stderr, "ERROR: %s destination pile not valid\n", __func__);
        return 0;
    }

    struct Card *c = ArrayGet(src->cards, 0);
    if ( c ) {
        PileMoveCards(dst, c);
    }

    return 0;
}

int MoonCardToTable(lua_State *L)
{
    struct Card *const c = lua_touserdata(L, 1);

    if ( !CardValid(c) ) {
        fprintf(stderr, "WARNING: %s card not valid\n", __func__);
        return 0;
    }
    MoonPushCardAsTable(L, c);
    return 1;
}