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

struct Cell* CellNew(struct Baize *const baize, Vector2 pos, enum FanType fan);
_Bool CellCanMoveTail(struct Array *const tail);
_Bool CellCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
_Bool CellCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
int CellCollect(struct Pile *const self);
_Bool CellComplete(struct Pile *const self);

#endif
