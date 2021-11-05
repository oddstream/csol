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
    &LabelCanMoveTail,
    &LabelCanAcceptCard,
    &LabelCanAcceptTail,
    &LabelTapped,
    &LabelCollect,
    &LabelComplete,
    &LabelConformant,
    &LabelSetRecycles,
    &LabelCountSortedAndUnsorted,

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

_Bool LabelCanMoveTail(struct Array *const tail)
{
    (void)tail;
    return 0;
}

_Bool LabelCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    (void)self;
    (void)c;
    BaizeSetError(baize, "(CSOL) Cannot move a card to a Label");
    return 0;
}

_Bool LabelCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    (void)self;
    (void)tail;
    BaizeSetError(baize, "(CSOL) Cannot move a card to a Label");
    return 0;
}

void LabelTapped(struct Pile *const self, struct Array *const tail)
{
    (void)self;
    (void)tail;
}

int LabelCollect(struct Pile *const self)
{
    (void)self;
    return 0;
}

_Bool LabelComplete(struct Pile *const self)
{
    (void)self;
    return 1;
}

_Bool LabelConformant(struct Pile *const self)
{
    (void)self;
    return 1;
}

void LabelSetRecycles(struct Pile *const self, int r)
{
    // we don't do that here
    (void)self;
    (void)r;
}

void LabelCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
    (void)self;
    (void)sorted;
    (void)unsorted;
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
    Vector2 pos = {
        .x= r.x + (r.width / 2.0f) - (mte.x / 2.0f),
        .y= r.y + (r.height / 2.0f) - (mte.y / 2.0f)
    };
    DrawTextEx(fontAcme, self->label, pos, fontSize, 1.2f, baizeHighlightColor);
}
