/* pile.h */

#ifndef PILE_H
#define PILE_H

#include <raylib.h>
#include "array.h"
#include "card.h"

enum FanType {
    NONE = 0,
    DOWN,
    LEFT,
    RIGHT,
    WASTE_DOWN,
    WASTE_LEFT,
    WASTE_RIGHT,
};

struct Pile {
    unsigned magic;
    char class[16];
    enum FanType fan;
    Vector2 pos;
    struct Array* cards;
};

struct Pile* PileNew(const char* class, Vector2 pos, enum FanType fan);
bool PileValid(struct Pile *const self);
size_t PileLen(struct Pile *const self);
void PilePushCard(struct Pile *const self, struct Card* c);
struct Card* PilePopCard(struct Pile *const self);
struct Card* PilePeekCard(struct Pile *const self);
void PileShuffle(struct Pile *const self);
Rectangle PileGetRect(struct Pile *const self);
Vector2 PileGetPos(struct Pile *const self);
void PileSetPos(struct Pile *const self, Vector2 pos);
Rectangle PileGetFannedRect(struct Pile *const self);
Vector2 PileGetPushedFannedPosition(struct Pile *const self);
void PileMoveCards(struct Pile *const self, struct Card* c);
void PileUpdate(struct Pile *const self);
void PileDraw(struct Pile *const self);
void PileFree(struct Pile *const self);

#endif
