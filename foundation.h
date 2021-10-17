/* foundation.h */

#ifndef FOUNDATION_H
#define FOUNDATION_H

#include <stdlib.h>
#include <raylib.h>
#include "array.h"
#include "card.h"
#include "pile.h"

struct Foundation {
    struct Pile super;
    enum CardOrdinal accept;
};

struct Foundation* FoundationNew(struct Baize *const baize, Vector2 pos, enum FanType fan);
bool FoundationCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
bool FoundationCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
int FoundationCollect(struct Pile *const self);
bool FoundationComplete(struct Pile *const self);
bool FoundationConformant(struct Pile *const self);
void FoundationSetAccept(struct Pile *const self, enum CardOrdinal ord);
void FoundationSetRecycles(struct Pile *const self, int r);
void FoundationCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted);
void FoundationDraw(struct Pile *const self);

#endif
