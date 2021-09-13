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
    struct Array* tail;
    Vector2 lastTouch;
};

struct Baize* BaizeNew(const char* variantName);
bool BaizeValid(struct Baize *const self);
struct Pile* BaizeLargestIntersection(struct Baize *const self, struct Card *const c);
void BaizeMakeTail(struct Baize *const self, struct Card *const cFirst);
void BaizeTouchStart(struct Baize *const self, Vector2 touchPosition);
void BaizeTouchMove(struct Baize *const self, Vector2 touchPosition);
void BaizeTouchStop(struct Baize *const self);
void BaizeUpdate(struct Baize *const self);
void BaizeDraw(struct Baize *const self);
void BaizeFree(struct Baize *const self);

#endif
