/* check.h */

#ifndef CHECK_H
#define CHECK_H

#include <stdio.h>
#include "array.h"
#include "pile.h"

bool CheckAccept(struct Baize *const baize, struct Pile *const dstPile, struct Card *const c);
// bool CheckCard(struct Baize *const baize, struct Pile *const dstPile, struct Card *const c);
bool CheckPair(struct Baize *const baize, struct Card *const cPrev, struct Card *const cNext);
bool CheckCards(struct Baize *const baize, struct Pile *const pile);
bool CheckTail(struct Baize *const baize, struct Array *const tail);

#endif