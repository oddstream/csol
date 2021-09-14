/* cell.h */

#ifndef CELL_H
#define CELL_H

#include <stdlib.h>
#include <raylib.h>
#include "array.h"
#include "card.h"
#include "pile.h"

struct Cell {
    struct Pile super;
};

struct Cell* CellNew(Vector2 pos, enum FanType fan);
bool CellCanAcceptTail(struct Pile *const self, struct Array *const tail);
void CellDraw(struct Pile *const self);

#endif
