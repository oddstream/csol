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

struct Stock* StockNew(Vector2 pos, enum FanType fanType, const char* buildfunc, const char* dragfunc);
bool StockCardTapped(struct Card *c);
bool StockPileTapped(struct Pile *p);
bool StockCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
void StockSetAccept(struct Pile *const self, enum CardOrdinal ord);
void StockSetRecycles(struct Pile *const self, int r);
void StockDraw(struct Pile *const self);

#endif
