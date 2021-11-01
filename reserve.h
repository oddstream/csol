/* reserve.h */

#ifndef RESERVE_H
#define RESERVE_H

#include <stdlib.h>
#include <raylib.h>
#include <lua.h>
#include "array.h"
#include "card.h"
#include "pile.h"

struct Reserve {
    struct Pile super;
};

struct Reserve* ReserveNew(struct Baize *const baize, Vector2 pos, enum FanType fan);
_Bool ReserveCanMoveTail(struct Array *const tail);
_Bool ReserveCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
_Bool ReserveCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
void ReserveTapped(struct Pile *const self, struct Array *const tail);
int ReserveCollect(struct Pile *const self);
_Bool ReserveComplete(struct Pile *const self);
_Bool ReserveConformant(struct Pile *const self);
enum CardOrdinal ReserveAccept(struct Pile *const self);
void ReserveSetAccept(struct Pile *const self, enum CardOrdinal ord);
void ReserveSetRecycles(struct Pile *const self, int r);
void ReserveCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted);

#endif
