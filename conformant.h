/* conformant.h */

#ifndef CONFORMANT_H
#define CONFORMANT_H

#include <stdio.h>
#include "array.h"
#include "pile.h"

bool CheckAccept(struct Baize *const baize, struct Pile *const dstPile, struct Card *const c);
bool CheckCard(struct Baize *const baize, struct Pile *const dstPile, struct Card *const c);
bool CheckPair(struct Baize *const baize, struct Card *const cPrev, struct Card *const cNext);
bool CheckTail(struct Baize *const baize, struct Pile *const dstPile, struct Array *const tail);

#endif