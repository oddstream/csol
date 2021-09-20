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
    &StockCanAcceptTail,
    &StockSetAccept,
    &StockSetRecycles,

    &PileUpdate,
    &StockDraw,
    &PileFree,
};

struct Stock* StockNew(Vector2 slot, enum FanType fan, enum DragType drag, const char* buildfunc, const char* dragfunc)
{
    struct Stock* self = malloc(sizeof(struct Stock));
    if ( self ) {
        PileCtor((struct Pile*)self, "Stock", slot, fan, drag, buildfunc, dragfunc);
        self->super.vtable = &stockVtable;
        self->recycles = 9999;  // infinite by default
    }
    return self;
}

bool StockCardTapped(struct Card *c)
{
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
    baize->errorString[0] = '\0';
    // TODO transfer 1-3 cards to Waste
    // fprintf(stdout, "Stock card tapped\n");
    struct Pile *w = BaizeFindPile(baize, "Waste", 1);
    if ( w ) {
        if ( PileMoveCards(w, c) ) {
            return true;
        }
    }
    return false;
}

bool StockPileTapped(struct Pile *p)
{
    p->owner->errorString[0] = '\0';
    size_t cardsMoved = 0;
    // fprintf(stderr, "Stock pile tapped\n");
    struct Stock *s = (struct Stock*)p;
    if ( s->recycles > 0 ) {
        struct Pile* w = BaizeFindPile(p->owner, "Waste", 1);
        if ( w ) {
            while ( PileLen(w) > 0 ) {
                struct Card *c = PilePopCard(w);
                PilePushCard(p, c);
                cardsMoved++;
            }
        }
        s->recycles--;
    } else {
        strcpy(p->owner->errorString, "No more recycles");
    }
    return cardsMoved > 0;
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

void StockSetRecycles(struct Pile *const self, int r)
{
    struct Stock *s = (struct Stock *)self;
    s->recycles = r;
}

void StockDraw(struct Pile *const self)
{
    PileDraw(self);

    struct Stock* s = (struct Stock*)self;
    if ( s->recycles < 10 ) {
        // TODO draw recycle symbol
        char z[16];
        sprintf(z, "%d", s->recycles);
        // extern Font fontAcme;
        Vector2 pos = PileGetScreenPos(self);
        pos.x += 10;
        pos.y += 10;
        // DrawTextEx(fontAcme, ords[f->accept], pos, 16, 0, (Color){255,255,255,127});
        DrawText(z, (int)pos.x, (int)pos.y, 24, (Color){255,255,255,31});
    }
}
