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

struct Waste* WasteNew(Vector2 pos, enum FanType fan, const char* buildfunc, const char* dragfunc);
bool WasteCardTapped(struct Card *c);
bool WastePileTapped(struct Pile *p);
bool WasteCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
void WasteSetAccept(struct Pile *const self, enum CardOrdinal ord);
void WasteSetRecycles(struct Pile *const self, int r);
void WasteDraw(struct Pile *const self);

#endif
