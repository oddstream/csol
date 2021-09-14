/* foundation.c */

#include <stdlib.h>
#include <raylib.h>
#include "pile.h"
#include "array.h"
#include "foundation.h"

static struct PileVtable foundationVtable = {
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

    &FoundationCanAcceptTail,

    &PileUpdate,
    &FoundationDraw,
    &PileFree,
};

struct Foundation* FoundationNew(Vector2 pos, enum FanType fan) {
    struct Foundation* self = malloc(sizeof(struct Foundation));
    if ( self ) {
        PileCtor((struct Pile*)self, "Foundation", pos, fan);
        self->super.vtable = &foundationVtable;
        self->accept = 0; // accept any by default
    }
    return self;
}

bool FoundationCanAcceptTail(struct Pile *const self, struct Array *const tail) {
    (void)self;
    return ArrayLen(tail) == 1;
}

void FoundationDraw(struct Pile *const self) {
    PileDraw(self);
    // TODO draw accept symbol
}
