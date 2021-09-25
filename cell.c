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

struct Cell* CellNew(Vector2 slot, enum FanType fan, const char* buildfunc, const char* dragfunc)
{
    struct Cell* self = malloc(sizeof(struct Cell));
    if ( self ) {
        PileCtor((struct Pile*)self, "Cell", slot, fan, buildfunc, dragfunc);
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

bool CellCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    if ( ArrayLen(tail) != 1 ) {
        strncpy(baize->errorString, "Can only move single cards to a cell", MAX_BAIZEERRORSTRING);
        return false;
    }
    if ( PileLen(self) > 0 ) {
        strncpy(baize->errorString, "Can only move a card to an empty cell", MAX_BAIZEERRORSTRING);
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
