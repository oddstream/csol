/* .c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "label.h"

static struct PileVtable labelVtable = {
    &PileInertCanMoveTail,
    &PileInertCanMatchTail,
    &PileInertCanAcceptCard,
    &PileInertCanAcceptTail,
    &PileInertTapped,
    &PileInertCollect,
    &PileInertComplete,
    &PileInertConformant,
    &PileInertSetRecycles,
    &PileInertCountSortedAndUnsorted,

    &PileUpdate,
    &LabelDraw,
    &PileFree,
};

struct Label* LabelNew(struct Baize *const baize, Vector2 slot, enum FanType fan)
{
    struct Label* self = calloc(1, sizeof(struct Label));
    if ( self ) {
        PileCtor(baize, (struct Pile*)self, "Label", slot, fan);
        self->super.vtable = &labelVtable;
    }
    return self;
}

void LabelDraw(struct Pile *const self)
{
    if (self->label[0] == '\0') {
        return;
    }

    extern Font fontAcme;
    extern float cardWidth;
    extern Color baizeHighlightColor;

    float fontSize = cardWidth / 4.0f;
    Vector2 mte = MeasureTextEx(fontAcme, self->label, fontSize, 1.2f);
    // center the label in the Pile screen rect
    Rectangle r = PileScreenRect(self);
    Vector2 pos = UtilCenterTextInRectangle(r, mte.x, mte.y);
    DrawTextEx(fontAcme, self->label, pos, fontSize, 1.2f, baizeHighlightColor);
}
