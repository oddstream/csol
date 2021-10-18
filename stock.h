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

struct Stock* StockNew(struct Baize *const baize, Vector2 pos, enum FanType fanType, size_t packs, size_t suits, bool cardFilter[14]);
bool StockCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
bool StockCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
int StockCollect(struct Pile *const self);
bool StockComplete(struct Pile *const self);
bool StockConformant(struct Pile *const self);
void StockSetAccept(struct Pile *const self, enum CardOrdinal ord);
void StockSetRecycles(struct Pile *const self, int r);
void StockCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted);
void StockDraw(struct Pile *const self);

#endif
