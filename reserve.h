/* reserve.h */

#ifndef RESERVE_H
#define RESERVE_H

#include <stdlib.h>
#include <raylib.h>

#include "array.h"
#include "card.h"
#include "pile.h"

struct Reserve {
    struct Pile super;
};

struct Reserve* ReserveNew(struct Baize *const baize, Vector2 pos, enum FanType fan);
_Bool ReserveCanMoveTail(struct Array *const tail);
_Bool ReserveCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
_Bool ReserveCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
int ReserveCollect(struct Pile *const self);
_Bool ReserveComplete(struct Pile *const self);
int ReserveUnsortedPairs(struct Pile *const self);

#endif
