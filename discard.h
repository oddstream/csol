/* discard.h */

#ifndef DISCARD_H
#define DISCARD_H

#include <stdlib.h>
#include <raylib.h>
#include "array.h"
#include "card.h"
#include "pile.h"

struct Discard {
    struct Pile super;
    enum CardOrdinal accept;
};

struct Discard* DiscardNew(struct Baize *const baize, Vector2 pos, enum FanType fan);
_Bool DiscardCanMoveTail(struct Array *const tail);
_Bool DiscardCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
_Bool DiscardCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
void DiscardTapped(struct Pile *const self, struct Array *const tail);
int DiscardCollect(struct Pile *const self);
_Bool DiscardComplete(struct Pile *const self);
_Bool DiscardConformant(struct Pile *const self);
void DiscardSetRecycles(struct Pile *const self, int r);
void DiscardCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted);

#endif
