/* tableau.h */

#ifndef TABLEAU_H
#define TABLEAU_H

#include <stdlib.h>
#include <raylib.h>
#include "array.h"
#include "card.h"
#include "pile.h"

struct Tableau {
    struct Pile super;
    enum CardOrdinal accept;
};

struct Tableau* TableauNew(Vector2 pos, enum FanType fan);
bool TableauCanAcceptTail(struct Pile *const self, struct Array *const tail);
void TableauDraw(struct Pile *const self);

#endif
