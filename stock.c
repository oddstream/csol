/* stock.c */

#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include "baize.h"
#include "pile.h"
#include "array.h"
#include "stock.h"

static struct PileVtable stockVtable = {
    &StockCardTapped,
    &StockPileTapped,
    &StockCanAcceptTail,
    &StockSetAccept,

    &PileUpdate,
    &StockDraw,
    &PileFree,
};

struct Stock* StockNew(Vector2 pos, enum FanType fan, const char* buildfunc, const char* dragfunc)
{
    struct Stock* self = malloc(sizeof(struct Stock));
    if ( self ) {
        PileCtor((struct Pile*)self, "Stock", pos, fan, buildfunc, dragfunc);
        self->super.vtable = &stockVtable;
        self->recycles = 0;
    }
    return self;
}

void StockCardTapped(lua_State *L, struct Card *c)
{
    if ( !CardValid(c) ) {
        fprintf(stderr, "ERROR Card is not valid\n");
        return;
    }
    struct Pile *p = c->owner;
    if ( !PileValid(p) ) {
        fprintf(stderr, "ERROR Pile is not valid\n");
        return;
    }
    struct Baize *baize = p->owner;
    if ( !BaizeValid(baize) ) {
        fprintf(stderr, "ERROR Baize is not valid\n");
        return;
    }
    // TODO transfer 1-3 cards to Waste
    fprintf(stdout, "Stock card tapped\n");
    // find a Waste pile using baize = c->owner->owner, baize->piles
    (void)L;
    (void)c;
}

void StockPileTapped(lua_State *L, struct Pile *p)
{
    // TODO recycle
    fprintf(stderr, "Stock pile tapped\n");
    (void)L;
    (void)p;
}

bool StockCanAcceptTail(struct Pile *const self, lua_State *L, struct Array *const tail)
{
    (void)self;
    (void)L;
    (void)tail;
    return false;
}

void StockSetAccept(struct Pile *const self, enum CardOrdinal ord)
{
    (void)self;
    (void)ord;
}

void StockDraw(struct Pile *const self)
{
    PileDraw(self);
    // TODO draw recycle symbol
}
