/* constraint.h */

#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <stdio.h>
#include "array.h"
#include "pile.h"

bool CanTailBeMoved(struct Array *const tail);
bool CanTailBeAppended(struct Pile *const pile, struct Array *const tail);
bool IsPileConformant(struct Pile *const pile);

#endif