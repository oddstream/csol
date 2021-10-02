/* cell.h */

#ifndef CELL_H
#define CELL_H

#include <stdlib.h>
#include <raylib.h>
#include <lua.h>
#include "array.h"
#include "card.h"
#include "pile.h"

struct Cell {
    struct Pile super;
};

struct Cell* CellNew(Vector2 pos, enum FanType fan);
bool CellCardTapped(struct Card *c);
bool CellPileTapped(struct Pile *p);
bool CellCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
bool CellCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
int CellCollect(struct Pile *const self);
bool CellComplete(struct Pile *const self);
bool CellConformant(struct Pile *const self);
void CellSetAccept(struct Pile *const self, enum CardOrdinal ord);
void CellSetRecycles(struct Pile *const self, int r);
void CellCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted);

#endif
