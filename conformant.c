/* conformant.c */

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "conformant.h"
#include "array.h"
#include "pile.h"

static bool ConformantTail(lua_State *L, const char* func, struct Array* tail)
{
    if ( func == NULL || func[0] == '\0' ) return false;

    bool result = true;
    int typ = lua_getglobal(L, func);  // push value of Build onto the stack
    if ( typ != LUA_TFUNCTION ) {
        fprintf(stderr, "%s is not a function\n", func);
    } else {
        // push source pile catagory as first arg
        struct Card* c0 = ArrayGet(tail, 0);
        lua_pushstring(L, c0->owner->category);

        // build table on stack as second arg
        // outer table is an array/sequence, same length as tail
        // each inner table is a record of each card eg {ordinal=1, suit=SPADE, color=0, prone=false}
        lua_createtable(L, ArrayLen(tail), 0);  // create and push tail table

        size_t index;
        struct Card* c = ArrayFirst(tail, &index);
        while ( c ) {
            lua_createtable(L, 0, 4);       // create and push new 4-element table
            
            lua_pushinteger(L, c->id.ordinal);
            lua_setfield(L, -2, "ordinal"); // table["ordinal"] = c->ord, pops key value

            lua_pushinteger(L, c->id.suit);
            lua_setfield(L, -2, "suit");    // table["suit"] = c->suit, pops key value

            lua_pushinteger(L, c->id.suit == DIAMOND || c->id.suit == HEART ? 1 : 0);
            lua_setfield(L, -2, "color");   // table["color"] == [0|1], pops key value

            lua_pushboolean(L, c->id.prone);
            lua_setfield(L, -2, "prone");   // table["prone"] = c->prone, pops key value

            lua_seti(L, -2, index + 1);
        
            c = ArrayNext(tail, &index);
        }
        // two args (source pile, table of card tables), one return (boolean)
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

    return result;
}

bool ConformantBuildTail(lua_State *L, struct Pile *const pile, struct Array *tail)
{
    if ( pile->buildfunc[0] == '\0' ) {
        return false;
    }
    if ( ArrayLen(tail) == 0 ) {
        fprintf(stderr, "WARNING: ConformantBuildTail passed empty tail\n");
        return false;
    }
    return ConformantTail(L, pile->buildfunc, tail);
}

bool ConformantBuildAppend(lua_State *L, struct Pile *const pile, struct Array *tail)
{
    if ( ArrayLen(tail) == 0 ) {
        fprintf(stderr, "WARNING: empty tail passed to ConformantBuildAppend\n");
        return false;
    }
    struct Card *c = PilePeekCard(pile);
    if ( !c ) {
        fprintf(stderr, "WARNING: ConformantBuildAppend onto an empty pile\n");
        return false;
    }

    struct Array *t2 = ArrayNew(1 + ArrayLen(tail));
    ArrayPush(t2, c);

    size_t index;
    c = ArrayFirst(tail, &index);
    while ( c ) {
        ArrayPush(t2, c);
        c = ArrayNext(tail, &index);
    }

    bool result = ConformantTail(L, pile->buildfunc, t2);
    ArrayFree(t2);
    return result;
}

bool ConformantDragTail(lua_State *L, struct Pile *const pile, struct Array* tail)
{
    if ( pile->dragfunc[0] == '\0' ) {
        return false;
    }
    if ( ArrayLen(tail) == 0 ) {
        fprintf(stderr, "WARNING: ConformantDragTail passed empty tail\n");
        return false;
    }
    if ( ArrayLen(tail) == 1 ) {
        return true;
    }

    return ConformantTail(L, pile->dragfunc, tail);
}

bool CheckDrag(struct Array* tail)
{
    struct Card* c0 = ArrayGet(tail, 0);
    switch ( c0->owner->dragType ) {
        case DRAG_NONE:
            fprintf(stdout, "No dragging from %s\n", c0->owner->category);
            return false;
        case DRAG_SINGLE:
            if ( ArrayLen(tail) != 1 ) {
                fprintf(stdout, "Can only drag a single card from %s\n", c0->owner->category);
                return false;
            }
        case DRAG_SINGLEORPILE:
            // TODO
            break;
        case DRAG_MANY:
            break;
    }
    return true;
}
