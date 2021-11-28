/* label.h */

#ifndef LABEL_H
#define LABEL_H

#include <stdlib.h>
#include <raylib.h>

#include "array.h"
#include "card.h"
#include "pile.h"

struct Label {
    struct Pile super;
};

struct Label* LabelNew(struct Baize *const baize, Vector2 pos, enum FanType fan);
void LabelSetLabel(struct Pile *const self, const char* label);
void LabelDraw(struct Pile *const self);

#endif
