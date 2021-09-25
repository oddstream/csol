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

struct Foundation* FoundationNew(Vector2 pos, enum FanType fan, const char* buildfunc, const char* dragfunc);
bool FoundationCardTapped(struct Card *c);
bool FoundationPileTapped(struct Pile *p);
bool FoundationCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
void FoundationSetAccept(struct Pile *const self, enum CardOrdinal ord);
void FoundationSetRecycles(struct Pile *const self, int r);
void FoundationDraw(struct Pile *const self);

#endif
