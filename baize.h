/* baize.h */

#ifndef BAIZE_H
#define BAIZE_H

#include <lua.h>

#include "array.h"
#include "pile.h"

struct Baize {
    struct Card* cardLibrary;
    lua_State *L;
    struct Array piles;
    struct Pile* stock;
};

struct Baize* BaizeNew(const char* variantName);
void BaizeUpdate(struct Baize* b);
void BaizeDraw(struct Baize* b);
void BaizeFree(struct Baize *b);

#endif
