/* pile.h */

#ifndef PILE_H
#define PILE_H

#include <raylib.h>
#include "array.h"
#include "card.h"

struct Pile {
    Vector2 pos;
    struct Array cards;
};

struct Pile* PileNew(void);
void PilePush(struct Pile* p, struct Card* pc);
struct Card* PilePop(struct Pile* p);
struct Card* PilePeek(struct Pile* p);
void PileUpdate(struct Pile* p);
void PileDraw(struct Pile* p);
void PileFree(struct Pile* p);

#endif
