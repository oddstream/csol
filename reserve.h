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
bool ReserveCanMoveTail(struct Array *const tail);
bool ReserveCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
bool ReserveCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
void ReserveTapped(struct Pile *const self, struct Array *const tail);
int ReserveCollect(struct Pile *const self);
bool ReserveComplete(struct Pile *const self);
bool ReserveConformant(struct Pile *const self);
void ReserveSetAccept(struct Pile *const self, enum CardOrdinal ord);
void ReserveSetRecycles(struct Pile *const self, int r);
void ReserveCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted);

#endif
