/* waste.h */

#ifndef WASTE_H
#define WASTE_H

#include <stdlib.h>
#include <raylib.h>
#include <lua.h>
#include "array.h"
#include "card.h"
#include "pile.h"

struct Waste {
    struct Pile super;
};

struct Waste* WasteNew(Vector2 pos, enum FanType fan);
bool WasteCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
bool WasteCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
int WasteCollect(struct Pile *const self);
bool WasteComplete(struct Pile *const self);
bool WasteConformant(struct Pile *const self);
void WasteSetAccept(struct Pile *const self, enum CardOrdinal ord);
void WasteSetRecycles(struct Pile *const self, int r);
void WasteCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted);

#endif
