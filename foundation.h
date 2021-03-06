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
};

struct Foundation* FoundationNew(struct Baize *const baize, Vector2 pos, enum FanType fan);
_Bool FoundationCanMoveTail(struct Array *const tail);
_Bool FoundationCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
_Bool FoundationCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
_Bool FoundationComplete(struct Pile *const self);

#endif
