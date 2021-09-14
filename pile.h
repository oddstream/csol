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

struct PileVtable;  // forward declaration

struct Pile {
    struct PileVtable *vtable;
    unsigned magic;
    char category[16];
    enum FanType fan;
    Vector2 pos;
    struct Array* cards;
};

struct PileVtable {
    bool (*Valid)(struct Pile *const self);
    size_t (*Len)(struct Pile *const self);

    void (*PushCard)(struct Pile *const self, struct Card* c);
    struct Card* (*PopCard)(struct Pile *const self);
    struct Card* (*PeekCard)(struct Pile *const self);
    void (*MoveCards)(struct Pile *const self, struct Card* c);

    Rectangle (*GetRect)(struct Pile *const self);
    Vector2 (*GetPos)(struct Pile *const self);
    void (*SetPos)(struct Pile *const self, Vector2 pos);
    Rectangle (*GetFannedRect)(struct Pile *const self);
    Vector2 (*GetPushedFannedPos)(struct Pile *const self);
    
    bool (*CanAcceptTail)(struct Pile *const self, struct Array *const tail);

    void (*Update)(struct Pile *const self);
    void (*Draw)(struct Pile *const self);
    void (*Free)(struct Pile *const self);
};

// struct Pile* PileNew(const char* category, Vector2 pos, enum FanType fan);
void PileCtor(struct Pile *const self, const char* category, Vector2 pos, enum FanType fan);
bool PileValid(struct Pile *const self);
size_t PileLen(struct Pile *const self);
void PilePushCard(struct Pile *const self, struct Card* c);
struct Card* PilePopCard(struct Pile *const self);
struct Card* PilePeekCard(struct Pile *const self);
Rectangle PileGetRect(struct Pile *const self);
Vector2 PileGetPos(struct Pile *const self);
void PileSetPos(struct Pile *const self, Vector2 pos);
Rectangle PileGetFannedRect(struct Pile *const self);
Vector2 PileGetPushedFannedPos(struct Pile *const self);
void PileMoveCards(struct Pile *const self, struct Card* c);
void PileUpdate(struct Pile *const self);
void PileDraw(struct Pile *const self);
void PileFree(struct Pile *const self);

#endif
