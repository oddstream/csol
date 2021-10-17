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
    enum CardOrdinal accept;
};

struct Tableau* TableauNew(struct Baize *const baize, Vector2 pos, enum FanType fan);
bool TableauCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
bool TableauCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
int TableauCollect(struct Pile *const self);
bool TableauComplete(struct Pile *const self);
bool TableauConformant(struct Pile *const self);
void TableauSetAccept(struct Pile *const self, enum CardOrdinal ord);
void TableauSetRecycles(struct Pile *const self, int r);
void TableauCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted);
void TableauDraw(struct Pile *const self);

#endif
