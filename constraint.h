/* constraint.h */

#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <stdio.h>
#include "array.h"
#include "pile.h"

_Bool CanTailBeMoved(struct Array *const tail);
_Bool CanTailBeAppended(struct Pile *const pile, struct Array *const tail);
_Bool IsPileConformant(struct Pile *const pile);

#endif