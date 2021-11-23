/* freecell.c */

#include "exiface.h"

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

static int PileUnsortedPairs(struct Pile *const pile)
{
    (void)pile;
    return 0;
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

static const char* Wikipedia(struct Baize *const baize)
{
    (void)baize;
    return "https://en.wikipedia.org/wiki/FreeCell";
}

static struct ExecutionInterface freecellVtable = {
    &BuildPiles,
    &StartGame,
    &AfterMove,
    &TailMoveError,
    &TailAppendError,
    &PileUnsortedPairs,
    &TailTapped,
    &PileTapped,
    &PercentComplete,
    &Wikipedia,
};

struct ExecutionInterface* GetFreecellInterface(void)
{
    return &freecellVtable;
}