/* conformant.h */

#ifndef CONFORMANT_H
#define CONFORMANT_H

#include <stdio.h>
#include "array.h"
#include "pile.h"

bool ConformantBuild(struct Baize *const baize, struct Pile *const pileDst, struct Array *tail);
bool ConformantDrag(struct Baize *const baize, struct Array *tail);

#endif