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

struct Stock* StockNew(Vector2 pos, enum FanType fan);
bool StockCanAcceptTail(struct Pile *const self, struct Array *const tail);
void StockDraw(struct Pile *const self);

#endif
