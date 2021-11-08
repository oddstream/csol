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
    &PileInertCanAcceptCard,
    &PileInertCanAcceptTail,
    &PileInertPileTapped,
    &PileInertTailTapped,
    &PileInertCollect,
    &PileInertComplete,
    &PileInertConformant,
    &LabelSetLabel,
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

void LabelSetLabel(struct Pile *const self, const char  *label)
{
    extern Font fontAcme;
    extern int pileFontSize;
    extern float fontSpacing;

    // fprintf(stdout, "INFO: %s: set %s label to '%s'\n", __func__, self->category, label);

    if (pileFontSize==0) fprintf(stderr, "ERROR: %s: pileFontSize is zero\n", __func__);
    memset(self->label, 0, MAX_PILE_LABEL + 1);
    if (*label) {
        strncpy(self->label, label, MAX_PILE_LABEL);
        self->labelmte = MeasureTextEx(fontAcme, self->label, (float)pileFontSize / 3, fontSpacing);
    } else {
        self->labelmte = (Vector2){0};
    }
}

void LabelDraw(struct Pile *const self)
{
    if (self->label[0] == '\0') {
        return;
    }

    extern Font fontAcme;
    extern int pileFontSize;
    extern float fontSpacing;
    // extern Color baizeHighlightColor;

    // center the label in the Pile screen rect
    Rectangle r = PileScreenRect(self);
    Vector2 pos = UtilCenterTextInRectangle(r, self->labelmte.x, self->labelmte.y);
    DrawTextEx(fontAcme, self->label, pos, pileFontSize / 3, fontSpacing, WHITE);
}
