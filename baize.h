/* baize.h */

#ifndef BAIZE_H
#define BAIZE_H

#include <lua.h>

#include "array.h"
#include "pile.h"

enum GameState {
    VIRGIN = 0,
    STARTED,
    COMPLETE,
};

struct Baize {
    unsigned magic;
    Color backgroundColor;
    struct Card *cardLibrary;
    lua_State *L;
    struct Array *piles;
    struct Pile *stock;     // actually a struct Stock*
    struct Array *tail;
    Vector2 lastTouch;
    struct Pile *touchedPile;
};

struct SavedBaizeHeader {
    enum GameState state;
    unsigned checksum;
    struct Array *savedPiles;
};

struct Baize* BaizeNew(const char* variantName);
bool BaizeValid(struct Baize *const self);
struct Pile* BaizeFindPile(struct Baize* self, const char *category, int n);
void BaizeMakeTail(struct Baize *const self, struct Card *const cFirst);
void BaizeTouchStart(struct Baize *const self, Vector2 touchPosition);
void BaizeTouchMove(struct Baize *const self, Vector2 touchPosition);
void BaizeTouchStop(struct Baize *const self);
void BaizeUpdate(struct Baize *const self);
void BaizeDraw(struct Baize *const self);
void BaizeFree(struct Baize *const self);

#endif
