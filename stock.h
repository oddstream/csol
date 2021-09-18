/* stock.h */

#ifndef STOCK_H
#define STOCK_H

#include <stdlib.h>
#include <raylib.h>
#include "array.h"
#include "card.h"
#include "pile.h"

struct Stock {
    struct Pile super;
    int recycles;
};

struct Stock* StockNew(Vector2 pos, enum FanType fan, enum DragType drag, const char* buildfunc, const char* dragfunc);
void StockCardTapped(struct Card *c);
void StockPileTapped(struct Pile *p);
bool StockCanAcceptTail(struct Pile *const self, lua_State *L, struct Array *const tail);
void StockSetAccept(struct Pile *const self, enum CardOrdinal ord);
void StockSetRecycles(struct Pile *const self, int r);
void StockDraw(struct Pile *const self);

#endif
