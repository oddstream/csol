/* tableau.h */

#ifndef TABLEAU_H
#define TABLEAU_H

#include <stdlib.h>
#include <raylib.h>
#include <lua.h>
#include "array.h"
#include "card.h"
#include "pile.h"

struct Tableau {
    struct Pile super;
};

struct Tableau* TableauNew(struct Baize *const baize, Vector2 pos, enum FanType fan);
_Bool TableauCanMoveTail(struct Array *const tail);
_Bool TableauCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
_Bool TableauCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
int TableauCollect(struct Pile *const self);
_Bool TableauComplete(struct Pile *const self);
_Bool TableauConformant(struct Pile *const self);
void TableauCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted);

#endif
