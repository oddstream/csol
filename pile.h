/* pile.h */

#ifndef PILE_H
#define PILE_H

#include <raylib.h>
#include <lua.h>

#include "array.h"
#include "card.h"

enum FanType {
    FAN_NONE = 0,
    FAN_DOWN,
    FAN_LEFT,
    FAN_RIGHT,
    FAN_DOWN3,
    FAN_LEFT3,
    FAN_RIGHT3,
};

struct PileVtable;  // forward declaration

struct Pile {
    unsigned magic;
    struct PileVtable *vtable;
    struct Baize *owner;
    char category[16];
    bool draggable; // eg Discard, Foundation
    bool singleCardMove;    // eg Forty Thieves
    Vector2 slot;
    Vector2 pos;
    enum FanType fanType;
    struct Array *cards;
};

struct PileVtable {
    bool (*CanAcceptCard)(struct Baize *const baize, struct Pile *const self, struct Card *const c);
    bool (*CanAcceptTail)(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
    int (*Collect)(struct Pile *p);
    bool (*Complete)(struct Pile *p);
    bool (*Conformant)(struct Pile *p);
    void (*SetAccept)(struct Pile *const self, enum CardOrdinal ord);
    void (*SetRecycles)(struct Pile *const self, int r);
    void (*CountSortedAndUnsorted)(struct Pile *const self, int *sorted, int *unsorted);
    void (*Update)(struct Pile *const self);
    void (*Draw)(struct Pile *const self);
    void (*Free)(struct Pile *const self);
};

void PileCtor(struct Pile *const self, const char* category, Vector2 slot, enum FanType fan);
bool PileValid(struct Pile *const self);
bool PileHidden(struct Pile *const self);
bool PileEmpty(struct Pile *const self);
size_t PileLen(struct Pile *const self);
void PilePushCard(struct Pile *const self, struct Card* c);
struct Card* PilePopCard(struct Pile *const self);
struct Card* PilePeekCard(struct Pile *const self);
bool PileIsStock(struct Pile *const self);
Rectangle PileBaizeRect(struct Pile *const self);
Rectangle PileScreenRect(struct Pile *const self);
Vector2 PileBaizePos(struct Pile *const self);
Vector2 PileScreenPos(struct Pile *const self);
Vector2 PileCalculatePosFromSlot(struct Pile *const self);
Rectangle PileFannedBaizeRect(struct Pile *const self);
Rectangle PileFannedScreenRect(struct Pile *const self);
Vector2 PilePushedFannedPos(struct Pile *const self);
bool PileMoveCards(struct Pile *const self, struct Card* c);
bool PileIsAt(struct Pile *const self, Vector2 point);
void PileRepushAllCards(struct Pile *const self);
int PileGenericCollect(struct Pile *const self);
void PileUpdate(struct Pile *const self);
void PileDraw(struct Pile *const self);
void PileFree(struct Pile *const self);

#endif
