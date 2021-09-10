/* baize.h */

#ifndef BAIZE_H
#define BAIZE_H

#include <lua.h>

#include "array.h"
#include "pile.h"

struct Baize {
    unsigned magic;
    struct Card* cardLibrary;
    lua_State *L;
    struct Array* piles;
    struct Pile* stock;
};

struct Baize* BaizeNew(const char* variantName);
bool BaizeValid(struct Baize *const self);
void BaizeUpdate(struct Baize *const self);
void BaizeDraw(struct Baize *const self);
void BaizeFree(struct Baize *const self);

#endif
