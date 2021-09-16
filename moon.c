/* moon.c */

#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>

#include "baize.h"
#include "array.h"
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
    {"SetAccept", MoonSetAccept},
};

static struct Baize* getBaize(lua_State* L) {
    int typ = lua_getglobal(L, "BAIZE");    // push light userdata on the stack
    if ( typ != LUA_TLIGHTUSERDATA ) {
        fprintf(stderr, "global BAIZE is not light userdata\n");
    }
    struct Baize* baize = lua_touserdata(L, -1); // doesn't alter stack
    lua_pop(L, 1);  // pop light userdata
    if ( !BaizeValid(baize) ) {
        fprintf(stderr, "global BAIZE is not valid\n");
    }
    return baize;
}

void MoonRegisterFunctions(lua_State* L) {
    for ( size_t i=0; i<sizeof(FunctionsToRegister) / sizeof(struct FunctionToRegister); i++ ) {
        lua_pushcfunction(L, FunctionsToRegister[i].cFunction);
        lua_setglobal(L, FunctionsToRegister[i].luaFunction);
    }
}

int MoonGetGlobalInt(lua_State* L, const char* var, const int def) {
    int result = def;
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    int typ = lua_getglobal(L, var);    // pushes value onto stack
    if ( typ != LUA_TNUMBER ) {
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
    // if ( lua_gettop(L) ) {
    //     lua_pop(L, 1);
    // }
    // lua_settop(L, 0);
    fprintf(stderr, "%s=%d\n", var, result);
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    return result;
}

float MoonGetFieldNumber(lua_State* L, const char* key, const float def) {
    // assumes table is on top of stack
    float result = def;
    int isnum;
    int  typ = lua_getfield(L, -1, key);    // pushes onto the stack the value t[k], where t is the value at the given index
    if ( typ != LUA_TNUMBER ) {
        fprintf(stderr, "%s is not a number\n", key);
    } else {
        result = (float)lua_tonumberx(L, -1, &isnum);    // returns a lua_Number
        if ( !isnum ) {
            fprintf(stderr, "%s is not a number\n", key);
            result = def;
        }
    }
    lua_pop(L, 1);  // remove number
    return result;
}

int MoonAddPile(lua_State* L) {
    struct Baize* baize = getBaize(L);
    if ( !baize ) {
        return 0;
    }

    const char* category = lua_tostring(L, 1); // doesn't alter stack
    float x = lua_tonumber(L, 2); // doesn't alter stack
    float y = lua_tonumber(L, 3); // doesn't alter stack
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

int MoonDealUp(lua_State* L) {
    struct Baize* baize = getBaize(L);
    if ( !baize ) {
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

int MoonDealDown(lua_State* L) {
    struct Baize* baize = getBaize(L);
    if ( !baize ) {
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

int MoonFindPile(lua_State* L) {
    struct Baize* baize = getBaize(L);
    if ( !baize ) {
        return 0;
    }

    const char* category = lua_tostring(L, 1); // doesn't alter stack
    int n = lua_tointeger(L, 2); // doesn't alter stack

    struct Pile *p = BaizeFindPile(baize, category, n);
    if ( p ) {
        lua_pushlightuserdata(L, p);
        return 1;
    }

    return 0;
}

int MoonMovePileTo(lua_State* L) {

    struct Pile* p = lua_touserdata(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);

    if ( PileValid(p) ) {
        Vector2 oldPos = PileGetPos(p);
        Vector2 newPos = (Vector2){.x=x, .y=y};
        float dx = newPos.x - oldPos.x;
        float dy = newPos.y - oldPos.y;
        PileSetPos(p, newPos);
        size_t index;
        for ( struct Card* c = ArrayFirst(p->cards, &index); c; c = ArrayNext(p->cards, &index) ) {
            Vector2 oldCardPos = CardGetPos(c);
            Vector2 newCardPos = (Vector2){.x = oldCardPos.x + dx, .y = oldCardPos.y + dy};
            CardSetPos(c, newCardPos);
        }
    }

    return 0;
}

int MoonSetAccept(lua_State* L) {

    struct Pile* p = lua_touserdata(L, 1);
    enum CardOrdinal ord = lua_tointeger(L, 2);

    p->vtable->SetAccept(p, ord);

    return 0;
}

