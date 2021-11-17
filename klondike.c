/* klondike.c */

#include "game.h"

static void BuildPiles(struct Baize *const baize)
{
    (void)baize;
}

static void StartGame(struct Baize *const baize)
{
    (void)baize;
}

static void AfterMove(struct Baize *const baize)
{
    (void)baize;
}

static const char* TailMoveError(struct Array *const tail)
{
    (void)tail;
    return (void*)0;
}

static const char* TailAppendError(struct Pile *const pile, struct Array *const tail)
{
    (void)pile;
    (void)tail;
    return (void*)0;
}

static const char* PileConformantError(struct Pile *const pile)
{
    (void)pile;
    return (void*)0;
}

static void PileSortedAndUnsorted(struct Pile *const pile, int* sorted, int* unsorted)
{
    (void)pile;
    (void)sorted;
    (void)unsorted;
}

static void TailTapped(struct Array *const tail)
{
    (void)tail;
}

static void PileTapped(struct Pile *const pile)
{
    (void)pile;
}

static int PercentComplete(struct Baize *const baize)
{
    (void)baize;
    return 0;
}

static struct GameVtable klondikeVtable = {
    &BuildPiles,
    &StartGame,
    &AfterMove,
    &TailMoveError,
    &TailAppendError,
    &PileConformantError,
    &PileSortedAndUnsorted,
    &TailTapped,
    &PileTapped,
    &PercentComplete,
};

struct Game* KlondikeCtor(struct Baize *const baize)
{
    (void)baize;

    struct Game *game = calloc(1, sizeof(struct Game));
    if (!game) {
        return (void*)0;
    }
    game->vtable = &klondikeVtable;

    return game;
}