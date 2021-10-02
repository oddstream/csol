/* conformant.h */

#ifndef CONFORMANT_H
#define CONFORMANT_H

#include <stdio.h>
#include "array.h"
#include "pile.h"

bool Conformant(struct Baize *const baize, struct Pile* pileDst, const char* func, struct Array* tail);
bool ConformantBuild(struct Baize *const baize, struct Pile *const pileDst, struct Array *tail);
bool ConformantDrag(struct Baize *const baize, struct Array *tail);

bool CheckBuildPair(struct Baize *const baize, struct Card *const cPrev, struct Card *const cNext);
bool CheckBuildTail(struct Baize *const baize, struct Array *const tail);
bool CheckDragPair(struct Baize *const baize, struct Card *const cPrev, struct Card *const cNext);
bool CheckDragTail(struct Baize *const baize, struct Array *const tail);

#endif