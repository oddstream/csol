/* cell.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>

#include "baize.h"
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

struct Cell* CellNew(Vector2 slot, enum FanType fan, enum DragType drag, const char* buildfunc, const char* dragfunc)
{
    struct Cell* self = malloc(sizeof(struct Cell));
    if ( self ) {
        PileCtor((struct Pile*)self, "Cell", slot, fan, drag, buildfunc, dragfunc);
        self->super.vtable = &cellVtable;
    }
    return self;
}

bool CellCardTapped(struct Card *c)
{
    c->owner->owner->errorString[0] = '\0';
    return false;
}

bool CellPileTapped(struct Pile *p)
{
    p->owner->errorString[0] = '\0';
    return false;
}

bool CellCanAcceptTail(struct Pile *const self, lua_State *L, struct Array *const tail)
{
    (void)L;
    if ( ArrayLen(tail) != 1 ) {
        strcpy(self->owner->errorString, "Can only move single cards to a cell");
        return false;
    }
    if ( PileLen(self) > 0 ) {
        strcpy(self->owner->errorString, "Can only move a card to an empty cell");
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
