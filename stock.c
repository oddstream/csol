/* stock.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "stock.h"

static struct PileVtable stockVtable = {
    &StockCanAcceptCard,
    &StockCanAcceptTail,
    &StockCollect,
    &StockComplete,
    &StockConformant,
    &StockSetAccept,
    &StockSetRecycles,
    &StockCountSortedAndUnsorted,

    &PileUpdate,
    &StockDraw,
    &PileFree,
};

struct Stock* StockNew(Vector2 slot, enum FanType fan)
{
    struct Stock* self = calloc(1, sizeof(struct Stock));
    if ( self ) {
        PileCtor((struct Pile*)self, "Stock", slot, fan);
        self->super.vtable = &stockVtable;
        self->recycles = 9999;  // infinite by default
    }
    return self;
}

bool StockCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    BaizeSetError(baize, "You cannot move cards to the Stock");

    (void)baize;
    (void)self;
    (void)c;
    return false;
}

bool StockCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    BaizeSetError(baize, "You cannot move cards to the Stock");

    (void)baize;
    (void)self;
    (void)tail;
    return false;
}

int StockCollect(struct Pile *const self)
{
    (void)self;
    return 0;
}

bool StockComplete(struct Pile *const self)
{
    (void)self;
    return false;
}

bool StockConformant(struct Pile *const self)
{
    (void)self;
    return false;
}

void StockSetAccept(struct Pile *const self, enum CardOrdinal ord)
{
    (void)self;
    (void)ord;
}

void StockSetRecycles(struct Pile *const self, int r)
{
    struct Stock *s = (struct Stock *)self;
    s->recycles = r;
}

void StockCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
    (void)sorted;
    *unsorted += ArrayLen(self->cards);
}

void StockDraw(struct Pile *const self)
{
    extern Font fontAcme;
    extern Color baizeHighlightColor;

    PileDraw(self);

    struct Stock* s = (struct Stock*)self;
    if ( s->recycles < 10 ) {
        // TODO draw recycle symbol
        char z[16];
        sprintf(z, "%d", s->recycles);
        Vector2 pos = PileScreenPos(self);
        pos.x += 10;
        pos.y += 10;
        DrawTextEx(fontAcme, z, pos, 24, 0, baizeHighlightColor);
    }
}
