/* tableau.h */

#ifndef TABLEAU_H
#define TABLEAU_H

#include <stdlib.h>
#include <raylib.h>

#include "array.h"
#include "card.h"
#include "pile.h"

enum MoveType {
    MOVE_ANY = 0,
    MOVE_ONE = 1,
    MOVE_ONE_PLUS = 2,
    MOVE_ONE_OR_ALL = 3,
};

struct Tableau {
    struct Pile super;
    enum MoveType moveType;
};

struct Tableau* TableauNew(struct Baize *const baize, Vector2 pos, enum FanType fan, enum MoveType move);
_Bool TableauCanMoveTail(struct Array *const tail);
_Bool TableauCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
_Bool TableauCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
int TableauCollect(struct Pile *const self);
_Bool TableauComplete(struct Pile *const self);
int TableauUnsortedPairs(struct Pile *const self);

#endif
