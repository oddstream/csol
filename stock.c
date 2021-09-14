/* stock.c */

#include <stdlib.h>
#include <raylib.h>
#include "pile.h"
#include "array.h"
#include "stock.h"

static struct PileVtable stockVtable = {
    &PileValid,
    &PileLen,

    &PilePushCard,
    &PilePopCard,
    &PilePeekCard,
    &PileMoveCards,

    &PileGetRect,
    &PileGetPos,
    &PileSetPos,
    &PileGetFannedRect,
    &PileGetPushedFannedPos,

    &StockCanAcceptTail,

    &PileUpdate,
    &StockDraw,
    &PileFree,
};

struct Stock* StockNew(Vector2 pos, enum FanType fan) {
    struct Stock* self = malloc(sizeof(struct Stock));
    if ( self ) {
        PileCtor((struct Pile*)self, "Stock", pos, fan);
        self->super.vtable = &stockVtable;
        self->recycles = 0;
    }
    return self;
}

bool StockCanAcceptTail(struct Pile *const self, struct Array *const tail) {
    (void)self;
    (void)tail;
    return false;
}

void StockDraw(struct Pile *const self) {
    PileDraw(self);
    // TODO draw recycle symbol
}
