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

struct Foundation* FoundationNew(Vector2 pos, enum FanType fan);
bool FoundationCanAcceptTail(struct Pile *const self, struct Array *const tail);
void FoundationDraw(struct Pile *const self);

#endif
