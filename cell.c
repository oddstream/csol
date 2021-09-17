/* cell.c */

#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <lua.h>

#include "pile.h"
#include "array.h"
#include "cell.h"

static struct PileVtable cellVtable = {
    &CellCardTapped,
    &CellPileTapped,
    &CellCanAcceptTail,
    &CellSetAccept,
    &CellSetRecycles,

    &PileUpdate,
    &PileDraw,
    &PileFree,
};

struct Cell* CellNew(Vector2 pos, enum FanType fan, const char* buildfunc, const char* dragfunc)
{
    struct Cell* self = malloc(sizeof(struct Cell));
    if ( self ) {
        PileCtor((struct Pile*)self, "Cell", pos, fan, buildfunc, dragfunc);
        self->super.vtable = &cellVtable;
    }
    return self;
}

void CellCardTapped(struct Card *c)
{
    (void)c;
}

void CellPileTapped(struct Pile *p)
{
    (void)p;
}

bool CellCanAcceptTail(struct Pile *const self, lua_State *L, struct Array *const tail)
{
    (void)L;
    if ( ArrayLen(tail) != 1 ) {
        fprintf(stderr, "Can only move single cards to a cell\n");
        return false;
    }
    if ( PileLen(self) > 0 ) {
        fprintf(stderr, "Can only move a card to an empty cell\n");
        return false;
    }
    return true;
}

void CellSetAccept(struct Pile *const self, enum CardOrdinal ord)
{
    // we don't do that here
    (void)self;
    (void)ord;
}

void CellSetRecycles(struct Pile *const self, int r)
{
    // we don't do that here
    (void)self;
    (void)r;
}
