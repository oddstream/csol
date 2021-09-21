/* baize.h */

#ifndef BAIZE_H
#define BAIZE_H

#include <lua.h>

#include "array.h"
#include "pile.h"

struct Baize {
    unsigned magic;
    Color backgroundColor;
    struct Card *cardLibrary;
    lua_State *L;

    struct Array *piles;
    struct Pile *stock;     // actually a struct Stock*
    struct Pile *waste;
    struct Array* foundations;
    struct Array* tableaux;

    struct Array *tail;
    struct Array *undoStack;
    Vector2 lastTouch;
    struct Pile *touchedPile;

    bool dragging;
    Vector2 dragOffset;

    char errorString[128];
};

struct Baize* BaizeNew(const char* variantName);
bool BaizeValid(struct Baize *const self);
void BaizePositionPiles(struct Baize *const self);
struct Pile* BaizeFindPile(struct Baize* self, const char *category, int n);
void BaizeMakeTail(struct Baize *const self, struct Card *const cFirst);
bool BaizeDragging(struct Baize *const self);
void BaizeStartDrag(struct Baize *const self);
void BaizeDragBy(struct Baize *const self, Vector2 delta);
void BaizeStopDrag(struct Baize *const self);
void BaizeTouchStart(struct Baize *const self, Vector2 touchPosition);
void BaizeTouchMove(struct Baize *const self, Vector2 touchPosition);
void BaizeTouchStop(struct Baize *const self);
void BaizeAfterUserMove(struct Baize *const self);
void BaizeUpdate(struct Baize *const self);
void BaizeDraw(struct Baize *const self);
void BaizeFree(struct Baize *const self);

#endif
