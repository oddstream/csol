/* pile.h */

#ifndef PILE_H
#define PILE_H

#include <raylib.h>
#include "array.h"
#include "card.h"

enum FanType {
    NONE = 0,
    RIGHT,
    LEFT,
    DOWN,
    WASTE_RIGHT,
    WASTE_LEFT,
    WASTE_DOWN
};

struct Pile {
    char class[16];
    enum FanType fan;
    Vector2 pos;
    struct Array cards;
};

struct Pile* PileNew(const char* class, Vector2 pos, enum FanType fan);
void PilePush(struct Pile* p, struct Card* pc);
struct Card* PilePop(struct Pile* p);
struct Card* PilePeek(struct Pile* p);
void PileUpdate(struct Pile* p);
void PileDraw(struct Pile* p);
void PileFree(struct Pile* p);

#endif
