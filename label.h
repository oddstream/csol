/* label.h */

#ifndef LABEL_H
#define LABEL_H

#include <stdlib.h>
#include <raylib.h>
#include <lua.h>
#include "array.h"
#include "card.h"
#include "pile.h"

struct Label {
    struct Pile super;
};

struct Label* LabelNew(struct Baize *const baize, Vector2 pos, enum FanType fan);
_Bool LabelCanMoveTail(struct Array *const tail);
_Bool LabelCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
_Bool LabelCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
void LabelTapped(struct Pile *const self, struct Array *const tail);
int LabelCollect(struct Pile *const self);
_Bool LabelComplete(struct Pile *const self);
_Bool LabelConformant(struct Pile *const self);
void LabelSetRecycles(struct Pile *const self, int r);
void LabelCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted);
void LabelDraw(struct Pile *const self);

#endif
