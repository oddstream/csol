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
    BaizeSetError(baize, "(C) You cannot move cards to the Stock");

    (void)baize;
    (void)self;
    (void)c;
    return false;
}

bool StockCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    BaizeSetError(baize, "(C) You cannot move cards to the Stock");

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
    return PileEmpty(self);
}

bool StockConformant(struct Pile *const self)
{
    return PileEmpty(self);
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
    extern float cardWidth, cardHeight;
    extern Texture2D recycleTexture;
    extern Color baizeHighlightColor;
    // extern Font fontAcme;

    PileDraw(self);

    // icon is 96x96
    // card is, say, 100x120

    struct Stock* s = (struct Stock*)self;
    if (s->recycles) {
        Vector2 pos = PileScreenPos(self);
        pos.x += (cardWidth - 96.0f) / 2.0f;
        pos.y += (cardHeight - 96.0f) / 2.0f;
        if ( CheckCollisionPointRec(GetMousePosition(), (Rectangle){.x=pos.x, .y=pos.y, .width=96.0f, .height=96.0f}) ) {
            pos.x += 2.0f;
            pos.y += 2.0f;
        }
        DrawTextureEx(
            recycleTexture,
            pos,
            0.0f,   // rotation
            1.0f,   // scale
            baizeHighlightColor
        );
        // if (s->recycles<10) {
        //     char str[16]; sprintf(str, "%d", s->recycles);
        //     Vector2 mte = MeasureTextEx(fontAcme, str, 24.0f, 0.0f);
        //     pos = PileScreenPos(self);
        //     pos.x += (cardWidth - mte.x) / 2.0f;
        //     pos.y += (cardHeight - mte.y) / 2.0f;
        //     DrawTextEx(fontAcme, str, pos, 24.0f, 0.0f, baizeHighlightColor);
        // }
    }
}
