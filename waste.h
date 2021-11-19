/* waste.h */

#ifndef WASTE_H
#define WASTE_H

#include <stdlib.h>
#include <raylib.h>

#include "array.h"
#include "card.h"
#include "pile.h"

struct Waste {
    struct Pile super;
};

struct Waste* WasteNew(struct Baize *const baize, Vector2 pos, enum FanType fan);
_Bool WasteCanMoveTail(struct Array *const tail);
_Bool WasteCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
_Bool WasteCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
int WasteCollect(struct Pile *const self);
_Bool WasteComplete(struct Pile *const self);
_Bool WasteConformant(struct Pile *const self);
void WasteCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted);

#endif
