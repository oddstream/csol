/* tableau.c */

#include <stdlib.h>
#include <raylib.h>
#include "pile.h"
#include "array.h"
#include "tableau.h"

static struct PileVtable tableauVtable = {
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

    &TableauCanAcceptTail,

    &PileUpdate,
    &TableauDraw,
    &PileFree,
};

struct Tableau* TableauNew(Vector2 pos, enum FanType fan) {
    struct Tableau* self = malloc(sizeof(struct Tableau));
    if ( self ) {
        PileCtor((struct Pile*)self, "Tableau", pos, fan);
        self->super.vtable = &tableauVtable;
        self->accept = 0;   // accept any by default
    }
    return self;
}

bool TableauCanAcceptTail(struct Pile *const self, struct Array *const tail) {
    (void)self;
    (void)tail;
    return true;
}

void TableauDraw(struct Pile *const self) {
    PileDraw(self);
    // TODO draw accept symbol
}
