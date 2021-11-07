/* tableau.c */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <raylib.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "tableau.h"
#include "constraint.h"
#include "luautil.h"
#include "util.h"

static struct PileVtable tableauVtable = {
    &TableauCanMoveTail,
    &TableauCanAcceptCard,
    &TableauCanAcceptTail,
    &PileInertPileTapped,
    &PileGenericTailTapped,
    &TableauCollect,
    &TableauComplete,
    &TableauConformant,
    &PileInertSetRecycles,
    &TableauCountSortedAndUnsorted,

    &PileUpdate,
    &PileDraw,
    &PileFree,
};

struct Tableau* TableauNew(struct Baize *const baize, Vector2 slot, enum FanType fan)
{
    struct Tableau* self = calloc(1, sizeof(struct Tableau));
    if ( self ) {
        PileCtor(baize, (struct Pile*)self, "Tableau", slot, fan);
        self->super.vtable = &tableauVtable;
    }
    return self;
}

_Bool TableauCanMoveTail(struct Array *const tail)
{
    return CanTailBeMoved(tail);
}

_Bool TableauCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    (void)baize;

    struct Array1 tail = Array1New(c);
    return CanTailBeAppended(self, (struct Array*)&tail);
    // don't need to free an Array1
}

static size_t PowerMoves(struct Baize *const self, struct Pile *const dstPile)
{
    double emptyCells = 0.0;
    double emptyCols = 0.0;
    size_t index;
    for ( struct Pile *p=ArrayFirst(self->piles, &index); p; p=ArrayNext(self->piles, &index) ) {
        if ( ArrayLen(p->cards) == 0 ) {
            if ( strcmp(p->category, "Cell") == 0 ) {
                emptyCells++;
            } else if ( strcmp(p->category, "Tableau") == 0 ) {
                // 'If you are moving into an empty column, then the column you are moving into does not count as empty column.'
                if ( p->label[0] == '\0' && p != dstPile ) {
                    emptyCols++;
                }
            }
        }
    }
    double n = (1.0 + emptyCells) * pow(2.0, emptyCols);
    return (size_t)n;
}

_Bool TableauCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    if (ArrayLen(tail) == 1) {
        return TableauCanAcceptCard(baize, self, ArrayGet(tail, 0));
    }
    if (ArrayLen(tail) > 1) {
        if (baize->powerMoves) {
            size_t moves = PowerMoves(baize, self);
            if ( ArrayLen(tail) > moves ) {
                char z[128];
                if ( moves == 1 )
                    sprintf(z, "(CSOL) Only enough space to move 1 card, not %lu", ArrayLen(tail));
                else
                    sprintf(z, "(CSOL) Only enough space to move %lu cards, not %lu", moves, ArrayLen(tail));
                BaizeSetError(baize, z);
                return 0;
            }
        }
    }
    return CanTailBeAppended(self, tail);
}

int TableauCollect(struct Pile *const self)
{
    return PileGenericCollect(self);
}

_Bool TableauComplete(struct Pile *const self)
{
    return PileEmpty(self);
}

_Bool TableauConformant(struct Pile *const self)
{
    return IsPileConformant(self);
}

void TableauCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
    // some optimizations
    if (PileEmpty(self)) {
        return;
    }
    if ( ArrayLen(self->cards) == 1 ) {
        *sorted += 1;
        return;
    }
#if 0
    (void)self;
    (void)sorted;
    (void)unsorted;
#else
    struct Baize *const baize = PileOwner(self);
    lua_State *L = baize->L;

    if (LuaUtilSetupTableMethod(L, "Tableau", "SortedAndUnsorted")) {
        lua_pushlightuserdata(L, self);
        // one arg (pile), two returns (number, number)
        if ( lua_pcall(L, 1, 2, 0) != LUA_OK ) {
            fprintf(stderr, "ERROR: %s: running Lua function: %s\n", __func__, lua_tostring(L, -1));
            lua_pop(L, 1);  // remove error
        } else {
            if ( lua_isnumber(L, -2) && lua_isnumber(L, -1) ) {
                int extraSorted = lua_tonumber(L, -2);
                int extraUnsorted = lua_tonumber(L, -1);
                *sorted += extraSorted;
                *unsorted += extraUnsorted;
            } else {
                fprintf(stderr, "ERROR: %s: expecting two numbers\n", __func__);
            }
            lua_pop(L, 2);
        }
    }
#endif
}
