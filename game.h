/* game.h */

#include <stdlib.h>

#include "array.h"
#include "baize.h"
#include "pile.h"

struct GameVtable {
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
};

struct Game {
    struct GameVtable *vtable;
};

struct Game* MoonGameCtor(struct Baize *const baize);
struct Game* KlondikeCtor(struct Baize *const baize);
struct Game* FreecellCtor(struct Baize *const baize);

void GameDtor(struct Game *game);