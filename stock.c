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
    &StockCardTapped,
    &StockPileTapped,
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

bool StockCardTapped(struct Card *c)
{   // not used
    if ( !CardValid(c) ) {
        fprintf(stderr, "ERROR Card is not valid\n");
        return false;
    }
    struct Pile *p = c->owner;
    if ( !PileValid(p) ) {
        fprintf(stderr, "ERROR Pile is not valid\n");
        return false;
    }
    struct Baize *baize = p->owner;
    if ( !BaizeValid(baize) ) {
        fprintf(stderr, "ERROR Baize is not valid\n");
        return false;
    }
    BaizeResetError(baize);
    // TODO transfer 1-3 cards to Waste
    // fprintf(stdout, "Stock card tapped\n");
    if ( baize->waste ) {
        if ( PileMoveCards(baize->waste, c) ) {
            return true;
        }
    }
    return false;
}

bool StockPileTapped(struct Pile *p)
{
    BaizeResetError(p->owner);
    size_t cardsMoved = 0;
    // fprintf(stderr, "Stock pile tapped\n");
    struct Stock *s = (struct Stock*)p;
    if ( s->recycles > 0 ) {
        if ( p->owner->waste ) {
            while ( PileLen(p->owner->waste) > 0 ) {
                struct Card *c = PilePopCard(p->owner->waste);
                PilePushCard(p, c);
                cardsMoved++;
            }
        }
        s->recycles--;
    } else {
        BaizeSetError(p->owner, "No more recycles");
    }
    return cardsMoved > 0;
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
