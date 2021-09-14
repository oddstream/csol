/* cell.c */

#include <stdlib.h>
#include <raylib.h>
#include "pile.h"
#include "array.h"
#include "cell.h"

static struct PileVtable cellVtable = {
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

    &CellCanAcceptTail,

    &PileUpdate,
    &PileDraw,
    &PileFree,
};

struct Cell* CellNew(Vector2 pos, enum FanType fan) {
    struct Cell* self = malloc(sizeof(struct Cell));
    if ( self ) {
        PileCtor((struct Pile*)self, "Cell", pos, fan);
        self->super.vtable = &cellVtable;
    }
    return self;
}

bool CellCanAcceptTail(struct Pile *const self, struct Array *const tail) {
    (void)tail;
    return PileLen(self) == 0 && ArrayLen(tail) == 1;
}
