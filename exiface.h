/* exiface.h */

#ifndef GAME_H
#define GAME_H

#include <stdlib.h>

#include "array.h"
#include "baize.h"
#include "pile.h"

struct ExecutionInterface {
    void (*BuildPiles)(struct Baize *const baize);
    void (*StartGame)(struct Baize *const baize);
    void (*AfterMove)(struct Baize *const baize);

    const char* (*TailMoveError)(struct Array *const tail);
    const char* (*TailAppendError)(struct Pile *const pile, struct Array *const tail);
    const char* (*PileConformantError)(struct Pile *const pile);
    void (*PileSortedAndUnsorted)(struct Pile *const pile, int* sorted, int* unsorted);

    void (*TailTapped)(struct Array *const tail);
    void (*PileTapped)(struct Pile *const pile);

    int (*PercentComplete)(struct Baize *const baize);
    // TODO add something to get/set? globals to/from Lua
    // if using a built-in, then we can get them directly
};

struct ExecutionInterface* GetInterface(struct Baize *const baize);

struct ExecutionInterface* GetMoonInterface(void);

struct ExecutionInterface* GetFallbackInterface(void);
struct ExecutionInterface* GetFreecellInterface(void);
struct ExecutionInterface* GetKlondikeInterface(void);

#endif
