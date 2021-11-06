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

struct Stock* StockNew(struct Baize *const baize, Vector2 pos, enum FanType fanType, size_t packs, size_t suits, _Bool cardFilter[14]);
_Bool StockCanMoveTail(struct Array *const tail);
_Bool StockCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
_Bool StockCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
_Bool StockComplete(struct Pile *const self);
_Bool StockConformant(struct Pile *const self);
void StockSetRecycles(struct Pile *const self, int r);
void StockCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted);
void StockDraw(struct Pile *const self);

#endif
