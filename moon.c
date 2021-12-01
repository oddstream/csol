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
#include "label.h"
#include "reserve.h"
#include "tableau.h"
#include "waste.h"
#include "moon.h"
#include "ui.h"

static const struct FunctionToRegister {
    char luaFunction[16];
    lua_CFunction cFunction;
} FunctionsToRegister[] = {
    {"AddPile",         MoonAddPile},
    // {"FindPile", MoonFindPile},
    {"PileLabel",       MoonPileLabel},
    {"PileType",        MoonPileType},
    // {"PileDemoteCards", MoonPileDemoteCards},
    // {"PilePromoteCards", MoonPilePromoteCards},

    {"MoveCard",        MoonMoveCard},
    {"MoveAllCards",    MoonMoveAllCards},
    {"Refan",           MoonRefan},
    {"CardPairs",       MoonCardPairs},
    {"SwapCards",       MoonSwapCards},

    {"CardColor",       MoonCardColor},
    {"CardOrdinal",     MoonCardOrdinal},
    {"CardOwner",       MoonCardOwner},
    {"CardProne",       MoonCardProne},
    {"CardSuit",        MoonCardSuit},
    // {"CardToTable", MoonCardToTable},

    {"Get",             MoonGet},
    {"Len",             MoonLen},
    {"First",           MoonFirst},
    {"Last",            MoonLast},
    {"Empty",           MoonEmpty},

    {"StockRecycles",   MoonStockRecycles},
    {"Toast",           MoonToast},
};

static struct Baize* getBaize(lua_State* L)
{
    int typ = lua_getglobal(L, "BAIZE");    // push light userdata on the stack
    if ( typ != LUA_TLIGHTUSERDATA ) {
        fprintf(stderr, "ERROR: %s: global BAIZE is not light userdata\n", __func__);
        lua_pop(L, 1);  // remove "BAIZE" from stack
        return NULL;
    }
    struct Baize* baize = lua_touserdata(L, -1); // doesn't alter stack
    lua_pop(L, 1);  // pop light userdata
    if ( !BaizeValid(baize) ) {
        fprintf(stderr, "ERROR: %s: global BAIZE is not valid\n", __func__);
        baize = NULL;
    }

#if 0
    {
        char buffer[64];
        sprintf(buffer, "%p", (void*)baize);

        struct Baize *b2 = NULL;
        sscanf(buffer, "%p", (void**)&b2);

        fprintf(stdout, "Baize is %p := %s := %p\n", (void*)baize, buffer, (void*)b2);
    }
#endif
    return baize;
}

void MoonRegisterFunctions(lua_State* L)
{
    // for ( int i = 0; i<ARRAY_SIZE(FunctionsToRegister); i++ ) {
   for ( size_t i=0; i<sizeof(FunctionsToRegister) / sizeof(struct FunctionToRegister); i++ ) {
        lua_pushcfunction(L, FunctionsToRegister[i].cFunction);
        lua_setglobal(L, FunctionsToRegister[i].luaFunction);
    }
}

#if 0
void MoonPushCardAsTable(lua_State *L, struct Card *const c)
{
    if ( !CardValid(c) ) {
        fprintf(stderr, "WARNING: %s: Invalid card\n", __func__);
        lua_pushnil(L);
    } else {
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

        lua_push_Boolean(L, c->prone);
        lua_setfield(L, -2, "prone");   // table["prone"] = c->prone, pops key value

        lua_pushlightuserdata(L, c->owner);
        lua_setfield(L, -2, "owner");   // table["owner"] = c->owner, pops key value
    }
}
#endif

#if 0
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
#endif

#if 0
void MoonPushArrayAsGlobalArray(lua_State *L, const char* name, struct Array *const a)
{
    lua_createtable(L, ArrayLen(a), 0);
    for ( size_t i=0; i<ArrayLen(a); i++ ) {
        lua_pushlightuserdata(L, ArrayGet(a, i));
        lua_seti(L, -2, i + 1); // pops lightuserdata from stack
        // lua_seti does the equivalent to t[n] = v,
        // where t is the value at the given index and v is the value on the top of the stack.
    }
    lua_setglobal(L, name);   // Pops a value from the stack and sets it as the new value of global name
}
#endif

static void parseCardFilter(lua_State *L, _Bool cardFilter[14])
{
    // on entry, the thing on the top of the stack (position 7) is a table

    for ( int i=1; i<14; i++ ) {
        lua_pushinteger(L, i);    // pushes +1 onto stack
        lua_gettable(L, -2);      // pops integer/index, pushes STRIP_CARDS[i]
        if ( lua_isnumber(L, -1) ) {
            int result;
            result = lua_tointeger(L, -1);    // doesn't alter stack
            if ( result > 0 && result < 14 ) {
                cardFilter[result] = 0;
                fprintf(stdout, "CardFilter: remove %d\n", result);
            } else {
                fprintf(stderr, "ERROR: cardFilter: invalid value %d\n", result);
            }
        }
        lua_pop(L, 1);    // remove result of lua_gettable
    }
}

int MoonAddPile(lua_State* L)
{
    struct Baize* baize = getBaize(L);
    if ( !BaizeValid(baize) ) {
        fprintf(stderr, "ERROR: %s: BAIZE not set\n", __func__);
        return 0;
    }

    if (!lua_isstring(L, 1)) {
        fprintf(stderr, "ERROR: %s: category string expected\n", __func__);
        return 0;
    }
    if (!lua_isnumber(L, 2)) {
        fprintf(stderr, "ERROR: %s: x number expected\n", __func__);
        return 0;
    }
    if (!lua_isnumber(L, 3)) {
        fprintf(stderr, "ERROR: %s: y number expected\n", __func__);
        return 0;
    }
    if (!lua_isinteger(L, 4)) {
        fprintf(stderr, "ERROR: %s: fan integer expected\n", __func__);
        return 0;
    }

    const char* category = lua_tostring(L, 1); // doesn't alter stack
    // the slot numbers in Lua start from 1, and start from 0 in C
    float x = lua_tonumber(L, 2) - 1.0f; // doesn't alter stack
    float y = lua_tonumber(L, 3) - 1.0f; // doesn't alter stack
    enum FanType fan = lua_tointeger(L, 4); // doesn't alter stack

    // fprintf(stderr, "PileNew(%s,%f,%f,%d)\n", category, x, y, fan);

    struct Pile* p = NULL;
    if ( strcmp(category, "Stock") == 0 ) {
        size_t packs, suits;
        if (lua_isinteger(L, 5)) {
            packs = lua_tointeger(L, 5);
        } else {
            packs = 1;
        }
        if (lua_isinteger(L, 6)) {
            suits = lua_tointeger(L, 6);
        } else {
            suits = 4;
        }
        _Bool cardFilter[14] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};
        if (lua_istable(L, 7)) {
            parseCardFilter(L, cardFilter);
        }
        p = (struct Pile*)StockNew(baize, (Vector2){x, y}, fan, packs, suits, cardFilter);
        if (PileValid(baize->stock)) {
            fprintf(stderr, "ERROR: %s: more than one Stock\n", __func__);
        }
        baize->stock = p;
    } else if ( strcmp(category, "Cell") == 0 ) {
        p = (struct Pile*)CellNew(baize, (Vector2){x, y}, fan);
    } else if ( strcmp(category, "Discard") == 0 ) {
        p = (struct Pile*)DiscardNew(baize, (Vector2){x, y}, fan);
    } else if ( strcmp(category, "Foundation") == 0 ) {
        p = (struct Pile*)FoundationNew(baize, (Vector2){x, y}, fan);
        baize->foundations = ArrayPush(baize->foundations, p);
    } else if ( strcmp(category, "Label") == 0 ) {
        p = (struct Pile*)LabelNew(baize, (Vector2){x, y}, fan);
    } else if ( strcmp(category, "Reserve") == 0 ) {
        p = (struct Pile*)ReserveNew(baize, (Vector2){x, y}, fan);
    } else if ( strcmp(category, "Tableau") == 0 ) {
        int moveType = 0;   // by default, MOVE_ANY
        if (lua_isinteger(L, 5)) {
            moveType = lua_tointeger(L, 5);
        }
        p = (struct Pile*)TableauNew(baize, (Vector2){x, y}, fan, moveType);
        baize->tableaux = ArrayPush(baize->tableaux, p);
    } else if ( strcmp(category, "Waste") == 0 ) {
        p = (struct Pile*)WasteNew(baize, (Vector2){x, y}, fan);
        if (PileValid(baize->waste)) {
            fprintf(stderr, "WARNING: %s: more than one Waste\n", __func__);
        }
        baize->waste = p;
    }

    if (!PileValid(p)) {
        fprintf(stderr, "ERROR: %s: invalid '%s' pile\n", __func__, category);
        return 0;
    }

    if (LUA_TTABLE != lua_getglobal(L, p->category)) {
        fprintf(stderr, "ERROR: %s: %s is not a table\n", __func__, p->category);
        lua_pop(L, 1);  // remove whatever that was
    } else {
        int npiles = BaizeCountPiles(baize, p->category);
        if (npiles == 0) {
            /*
                If this is the first pile of this type/category,
                then it could turn out to be the only one.
                So add it to the <category> table as <category>.Pile = <pile>
            */
            lua_pushlightuserdata(L, p);
            lua_setfield(L, -2, "Pile");    // table["Pile"] = p, pops key value

            lua_createtable(L, 0, 0);       // create and push empty table
            lua_setfield(L, -2, "Piles");   // table[Piles] = {}, pops key value
        }
        // the global <category> table should still be on the top of the stack
        if (LUA_TTABLE != lua_getfield(L, -1, "Piles")) {
            fprintf(stderr, "ERROR: %s: expected %s.Piles table\n", __func__, p->category);
            lua_pop(L, 2);  // pop category table and whatever that was
        } else {
            lua_pushlightuserdata(L, p);
            lua_seti(L, -2, npiles + 1);
        }
        // the global <category> table should still be on the top of the stack
        lua_pop(L, 1);
    }

    baize->piles = ArrayPush(baize->piles, p);

    lua_pushlightuserdata(L, p);
    return 1;
}

// don't need to find a pile if Lua is keeping track of piles itself in CELLS, TABLEAUX &c.
// int MoonFindPile(lua_State* L)
// {
//     struct Baize* baize = getBaize(L);
//     if ( !BaizeValid(baize) ) {
//         return 0;
//     }

//     const char* category = lua_tostring(L, 1); // doesn't alter stack
//     int n = lua_tointeger(L, 2); // doesn't alter stack

//     struct Pile *p = BaizeFindPile(baize, category, n);
//     if ( PileValid(p) ) {
//         lua_pushlightuserdata(L, p);
//         return 1;
//     }

//     return 0;
// }

int MoonPileLabel(lua_State *L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    struct Pile *const p = lua_touserdata(L, 1);
    if ( !PileValid(p) ) {
        fprintf(stderr, "ERROR: %s: invalid pile\n", __func__);
        return 0;
    }
    if (lua_gettop(L) == 2 && lua_isstring(L, 2)) {
        PileSetLabel(p, lua_tostring(L, 2));
    }
    lua_pushstring(L, p->label);
    return 1;
}

int MoonPileType(lua_State *L)
{
    if (lua_gettop(L)!=1) {
        fprintf(stderr, "WARNING: %s: gettop=%d\n", __func__, lua_gettop(L));
    }
    if (lua_type(L, 1) == LUA_TNIL) {
        fprintf(stderr, "WARNING: %s: pile is nil\n", __func__);
    }
    if (lua_type(L, 1) != LUA_TLIGHTUSERDATA) {
        fprintf(stderr, "WARNING: %s: pile is not lightuserdata\n", __func__);
    }
    struct Pile* p = lua_touserdata(L, 1);
    if (!p) {
        fprintf(stderr, "WARNING: %s: pile is NULL\n", __func__);
    }
    if ( !PileValid(p) ) {
        fprintf(stderr, "WARNING: %s: invalid pile, type=%d\n", __func__, lua_type(L, 1));
    } else {
        lua_pushstring(L, p->category);
        return 1;
    }
    return 0;
}

#if 0
int MoonPileDemoteCards(lua_State *L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    struct Pile *p = lua_touserdata(L, 1);
    if ( !PileValid(p) ) {
        fprintf(stderr, "ERROR: %s: invalid pile\n", __func__);
        return 0;
    }
    enum CardOrdinal ord = lua_tointeger(L, 2);
    if ( ord < 1 || ord > 13 ) {
        fprintf(stderr, "ERROR: %s: invalid ordinal\n", __func__);
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
            if (CardOrdinal(c) == ord) {
                p->cards = ArrayPush(p->cards, c);
            }
        }
        for ( struct Card *c=ArrayFirst(tmp, &index); c; c=ArrayNext(tmp, &index) ) {
            if (CardOrdinal(c) != ord) {
                p->cards = ArrayPush(p->cards, c);
            }
        }
        ArrayFree(tmp);
    }
    return 0;
}
#endif

#if 0
int MoonPilePromoteCards(lua_State *L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    struct Pile *p = lua_touserdata(L, 1);
    if ( !PileValid(p) ) {
        fprintf(stderr, "ERROR: %s: invalid pile\n", __func__);
        return 0;
    }
    enum CardOrdinal ord = lua_tointeger(L, 2);
    if ( ord < 1 || ord > 13 ) {
        fprintf(stderr, "ERROR: %s: invalid ordinal\n", __func__);
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
            if (CardOrdinal(c) != ord) {
                p->cards = ArrayPush(p->cards, c);
            }
        }
        for ( struct Card *c=ArrayFirst(tmp, &index); c; c=ArrayNext(tmp, &index) ) {
            if (CardOrdinal(c) == ord) {
                p->cards = ArrayPush(p->cards, c);
            }
        }
        ArrayFree(tmp);
    }
    return 0;
}
#endif

int MoonMoveCard(lua_State *L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "ERROR: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    if (!lua_islightuserdata(L, 2)) {
        fprintf(stderr, "ERROR: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    struct Pile *src = lua_touserdata(L, 1);
    struct Pile* dst = lua_touserdata(L, 2);

    if ( !PileValid(src) ) {
        fprintf(stderr, "ERROR: %s invalid source pile\n", __func__);
        return 0;
    }

    if ( !PileValid(dst) ) {
        fprintf(stderr, "ERROR: %s invalid destination pile\n", __func__);
        return 0;
    }

    struct Card *c = NULL;

    if (lua_gettop(L) == 4) {
        enum CardOrdinal ord = lua_tointeger(L, 3);
        enum CardSuit suit = lua_tointeger(L, 4);
        struct CardAndIndex ci = PileFindCard(src, ord, suit);
        if (ci.card) {
            ArrayDelete(src->cards, ci.index, NULL);
            PileRefan(src);
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
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    if (!lua_islightuserdata(L, 2)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    struct Pile *src = lua_touserdata(L, 1);
    struct Pile* dst = lua_touserdata(L, 2);

    if ( !PileValid(src) ) {
        fprintf(stderr, "ERROR: %s: invalid source pile\n", __func__);
        return 0;
    }

    if ( !PileValid(dst) ) {
        fprintf(stderr, "ERROR: %s: invalid destination pile\n", __func__);
        return 0;
    }

    struct Card *c = ArrayGet(src->cards, 0);
    if ( c ) {
        PileMoveCards(dst, c);
    }

    return 0;
}

int MoonRefan(lua_State *L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    struct Pile *pile = lua_touserdata(L, 1);

    if ( !PileValid(pile) ) {
        fprintf(stderr, "ERROR: %s: invalid source pile\n", __func__);
        return 0;
    }

    PileRefan(pile);

    return 0;
}

int MoonCardColor(lua_State *L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    struct Card *const c = lua_touserdata(L, 1);
    if ( !CardValid(c) ) {
        fprintf(stderr, "ERROR: %s: invalid card\n", __func__);
        lua_pushinteger(L, 0);
        return 1;
    }
    lua_pushinteger(L, CardSuit(c) == CLUB || CardSuit(c) == SPADE ? 0 : 1);
    return 1;
}

int MoonCardOrdinal(lua_State *L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    struct Card *const c = lua_touserdata(L, 1);
    if ( !CardValid(c) ) {
        fprintf(stderr, "ERROR: %s: invalid card\n", __func__);
        lua_pushinteger(L, 0);
        return 1;
    }
    lua_pushinteger(L, CardOrdinal(c));
    return 1;
}

int MoonCardOwner(lua_State *L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    struct Card* c = lua_touserdata(L, 1);
    if ( !CardValid(c) ) {
        fprintf(stderr, "ERROR: %s: invalid card\n", __func__);
        lua_pushnil(L);
        return 1;
    }
    lua_pushlightuserdata(L, CardOwner(c));
    return 1;
}

int MoonCardProne(lua_State *L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    struct Card *const c = lua_touserdata(L, 1);
    if ( !CardValid(c) ) {
        fprintf(stderr, "ERROR: %s: invalid card\n", __func__);
        lua_pushboolean(L, 0);
        return 1;
    }
    if (lua_gettop(L) == 2 && lua_isboolean(L, 2)) {
        _Bool prone = lua_toboolean(L, 2);
        if ( prone ) {
            CardFlipDown(c);
        } else {
            CardFlipUp(c);
        }
    }
    lua_pushboolean(L, CardProne(c));
    return 1;
}

int MoonCardSuit(lua_State *L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    struct Card *const c = lua_touserdata(L, 1);
    if ( !CardValid(c) ) {
        fprintf(stderr, "ERROR: %s: invalid card\n", __func__);
        lua_pushinteger(L, 0);
        return 1;
    }
    lua_pushinteger(L, CardSuit(c));
    return 1;
}

#if 0
int MoonCardToTable(lua_State *L)
{
    struct Card *const c = lua_touserdata(L, 1);

    if ( !CardValid(c) ) {
        fprintf(stderr, "WARNING: %s invalid card\n", __func__);
        return 0;
    }
    MoonPushCardAsTable(L, c);
    return 1;
}
#endif

int MoonLen(lua_State *L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    void *thing = lua_touserdata(L, 1);
    if (!thing) {
        fprintf(stderr, "WARNING: %s: invalid thing\n", __func__);
        return 0;
    }
    size_t len = 0;
    if (PileValid(thing)) {
        len = PileLen(thing);
    } else if (ArrayValid(thing)) {
        len = ArrayLen(thing);
    } else if (CardValid(thing)) {
        fprintf(stderr, "WARNING: %s: Len(Card) is a bit odd\n", __func__);
        len = 1;
    }
    lua_pushinteger(L, len);
    return 1;
}

int MoonGet(lua_State* L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    void* thing = lua_touserdata(L, 1);
    if (!thing) {
        fprintf(stderr, "WARNING: %s: invalid thing\n", __func__);
        return 0;
    }
    if (!lua_isnumber(L, 2)) {
        fprintf(stderr, "ERROR: %s: invalid index, have type %d\n", __func__, lua_type(L, 2));
    }
    int n = lua_tonumber(L, 2);
    if (n==0) {
        fprintf(stderr, "WARNING: %s: zero index\n", __func__);
    }
    // C is 0-indexed, Lua is 1-indexed
    struct Card *c = NULL;
    if (PileValid(thing)) {
        c = ArrayGet(((struct Pile*)thing)->cards, n-1);
    } else if (ArrayValid(thing)) {
        c = ArrayGet(thing, n-1);
    } else {
        fprintf(stderr, "WARNING: %s: unknown thing\n", __func__);
    }
    if (!CardValid(c)) {
        fprintf(stderr, "WARNING: %s: invalid card\n", __func__);
        return 0;
    }
    lua_pushlightuserdata(L, c);
    return 1;
}

int MoonFirst(lua_State* L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    void* thing = lua_touserdata(L, 1);
    if (!thing) {
        fprintf(stderr, "WARNING: %s: invalid thing\n", __func__);
        return 0;
    }

    struct Card *c = NULL;
    if (PileValid(thing)) {
        c = ArrayGet(((struct Pile*)thing)->cards, 0);
    } else if (ArrayValid(thing)) {
        c = ArrayGet(thing, 0);
    } else {
        fprintf(stderr, "WARNING: %s: unknown thing\n", __func__);
    }
    if (!CardValid(c)) {
        fprintf(stderr, "WARNING: %s: invalid card\n", __func__);
        return 0;
    }
    lua_pushlightuserdata(L, c);
    return 1;
}

int MoonLast(lua_State* L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    void* thing = lua_touserdata(L, 1);
    if (!thing) {
        fprintf(stderr, "WARNING: %s: invalid thing\n", __func__);
        return 0;
    }

    struct Card *c = NULL;
    if (PileValid(thing)) {
        c = ArrayPeek(((struct Pile*)thing)->cards);
    } else if (ArrayValid(thing)) {
        c = ArrayPeek(thing);
    } else {
        fprintf(stderr, "WARNING: %s: unknown thing\n", __func__);
    }
    if (!CardValid(c)) {
        fprintf(stderr, "WARNING: %s: invalid card\n", __func__);
        return 0;
    }
    lua_pushlightuserdata(L, c);
    return 1;
}

int MoonEmpty(lua_State *L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "WARNING: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    void *thing = lua_touserdata(L, 1);
    if (!thing) {
        fprintf(stderr, "WARNING: %s: invalid thing\n", __func__);
        return 0;
    }
    _Bool empty = 1;
    if (PileValid(thing)) {
        empty = PileEmpty(thing);
    } else if (ArrayValid(thing)) {
        empty = ArrayLen(thing) == 0;
    } else if (CardValid(thing)) {
        fprintf(stderr, "WARNING: %s: Empty(Card) is a bit odd\n", __func__);
        empty = 1;
    }
    lua_pushboolean(L, empty);
    return 1;
}

int MoonCardPairs(lua_State *L)
{
    if (!lua_islightuserdata(L, 1)) {
        fprintf(stderr, "ERROR: %s: expecting lightuserdata\n", __func__);
        return 0;
    }
    void *thing = lua_touserdata(L, 1);
    if (!thing) {
        fprintf(stderr, "ERROR: %s: null thing\n", __func__);
        return 0;
    }

    struct Array *cards = NULL;
    if (PileValid(thing)) {
        cards = ((struct Pile*)thing)->cards;
    } else if (ArrayValid(thing)) {
        cards = ((struct Array *)thing);
    } else {
        fprintf(stderr, "ERROR: %s: unknown thing\n", __func__);
        return 0;
    }
    lua_createtable(L, ArrayLen(cards), 0);
    if (ArrayLen(cards) > 1) {
        struct Card *c1 = ArrayGet(cards, 0);
        for ( size_t i=1; i<ArrayLen(cards); i++ ) {
            struct Card *c2 = ArrayGet(cards, i);
            lua_createtable(L, 2, 0);   // 2 array elements, 0 table elements
            lua_pushlightuserdata(L, c1);
            lua_seti(L, -2, 1);   // pops lightuserdata from stack
            lua_pushlightuserdata(L, c2);
            lua_seti(L, -2, 2);   // pops lightuserdata from stack
            c1 = c2;
            lua_seti(L, -2, i);   // we start from 1, which is a nice coincidence
        }
    }
    return 1; // we created 1 array table (of card pair array tables)
}

int MoonSwapCards(lua_State *L)
{
    struct Card *c1, *c2;
    struct Pile *p1, *p2;
    size_t i1, i2;

    if (!(lua_islightuserdata(L, 1) && lua_islightuserdata(L, 1))) {
        fprintf(stderr, "ERROR: %s: expecting two lightuserdata\n", __func__);
        return 0;
    }
    c1 = lua_touserdata(L, 1);
    c2 = lua_touserdata(L, 2);
    if (!(CardValid(c1) && CardValid(c2))) {
        fprintf(stderr, "ERROR: %s: expecting two cards\n", __func__);
        return 0;
    }
    p1 = CardOwner(c1);
    p2 = CardOwner(c2);
    ArrayIndexOf(p1->cards, c1, &i1);   // ignore _Bool return
    ArrayIndexOf(p2->cards, c2, &i2);   // ignore _Bool return

    ArrayPut(p1->cards, i1, c2);
    ArrayPut(p2->cards, i2, c1);

    c1->owner = p2;
    c2->owner = p1;

    return 0;
}

int MoonStockRecycles(lua_State *L)
{
    struct Baize *baize = getBaize(L);

    if (lua_isinteger(L, 1)) {
        baize->stock->vtable->SetRecycles(baize->stock, lua_tointeger(L, 1));
    }
    lua_pushinteger(L, ((struct Stock*)baize->stock)->recycles);
    return 1;
}

int MoonToast(lua_State *L)
{
    struct Baize *const baize = getBaize(L);
    if (baize) {
        if (lua_isstring(L, -1)) {
            UiToast(baize->ui, lua_tostring(L, -1));
        } else {
            fprintf(stderr, "WARNING: %s: string expected\n", __func__);
        }
    }
    return 0;
}