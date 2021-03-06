/* script.h */

#ifndef GAME_H
#define GAME_H

#include <stdlib.h>

#include "array.h"
#include "baize.h"
#include "pile.h"

struct ScriptInterface {
    void (*BuildPiles)(struct Baize *const baize);
    void (*StartGame)(struct Baize *const baize);
    void (*AfterMove)(struct Baize *const baize);

    const char* (*TailMoveError)(struct Array *const tail);
    const char* (*TailAppendError)(struct Pile *const pile, struct Array *const tail);
    int (*PileUnsortedPairs)(struct Pile *const pile);

    void (*TailTapped)(struct Array *const tail);
    void (*PileTapped)(struct Pile *const pile);

    int (*PercentComplete)(struct Baize *const baize);

    const char* (*Wikipedia)(void);
};

struct ScriptInterface* GetInterface(struct Baize *const baize);

struct ScriptInterface* GetMoonGameInterface(void);
struct ScriptInterface* GetClondikeInterface(void);
struct ScriptInterface* GetFreecellInterface(void);

const char* CardCompare_Up(struct Card *const c1, struct Card *const c2);
const char* CardCompare_Down(struct Card *const c1, struct Card *const c2);
const char* CardCompare_DownAltColor(struct Card *const c1, struct Card *const c2);
const char* CardCompare_DownAltColorWrap(struct Card *const c1, struct Card *const c2);
const char* CardCompare_UpAltColor(struct Card *const c1, struct Card *const c2);
const char* CardCompare_UpSuit(struct Card *const c1, struct Card *const c2);
const char* CardCompare_DownSuit(struct Card *const c1, struct Card *const c2);
const char* CardCompare_UpSuitWrap(struct Card *const c1, struct Card *const c2);
const char* CardCompare_DownSuitWrap(struct Card *const c1, struct Card *const c2);

#endif
