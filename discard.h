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
bool DiscardCanMoveTail(struct Array *const tail);
bool DiscardCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
bool DiscardCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
void DiscardTapped(struct Pile *const self, struct Array *const tail);
int DiscardCollect(struct Pile *const self);
bool DiscardComplete(struct Pile *const self);
bool DiscardConformant(struct Pile *const self);
void DiscardSetAccept(struct Pile *const self, enum CardOrdinal ord);
void DiscardSetRecycles(struct Pile *const self, int r);
void DiscardCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted);
void DiscardDraw(struct Pile *const self);

#endif
