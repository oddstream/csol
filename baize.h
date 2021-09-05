/* baize.h */

#ifndef BAIZE_H
#define BAIZE_H

#include "array.h"
#include "pile.h"

struct Baize {
    struct Array piles;
    struct Pile* stock;
};

struct Baize* BaizeNew(void);
void BaizeUpdate(struct Baize* b);
void BaizeDraw(struct Baize* b);
void BaizeFree(struct Baize *b);

#endif
