/* conformant.h */

#ifndef CONFORMANT_H
#define CONFORMANT_H

#include <stdio.h>
#include "array.h"
#include "pile.h"

bool ConformantBuild(struct Baize *const baize, struct Pile *const pile, struct Card *c, struct Array *tail);
bool ConformantDrag(struct Baize *const baize, struct Pile *const pile, struct Array *tail);

#endif