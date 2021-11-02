/* pile.h */

#pragma once

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
    Vector2 slot;   // pile baize position index
    Vector2 pos;    // baize coords (screen coords are calculated)
    enum FanType fanType;
    struct Array *cards;
};

struct PileVtable {
    _Bool (*CanMoveTail)(struct Array *const tail);
    _Bool (*CanAcceptCard)(struct Baize *const baize, struct Pile *const self, struct Card *const c);
    _Bool (*CanAcceptTail)(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
    void (*Tapped)(struct Pile *const self, struct Array *const tail);
    int (*Collect)(struct Pile *p);
    _Bool (*Complete)(struct Pile *p);
    _Bool (*Conformant)(struct Pile *p);
    enum CardOrdinal (*Accept)(struct Pile *const self);
    void (*SetAccept)(struct Pile *const self, enum CardOrdinal ord);
    void (*SetRecycles)(struct Pile *const self, int r);
    void (*CountSortedAndUnsorted)(struct Pile *const self, int *sorted, int *unsorted);
    void (*Update)(struct Pile *const self);
    void (*Draw)(struct Pile *const self);
    void (*Free)(struct Pile *const self);
};

struct CardAndIndex {struct Card* card; size_t index;};

void PileCtor(struct Baize *const baize, struct Pile *const self, const char* category, Vector2 slot, enum FanType fan);
_Bool PileValid(struct Pile *const self);
struct Baize* PileOwner(struct Pile *const self);
_Bool PileHidden(struct Pile *const self);
_Bool PileEmpty(struct Pile *const self);
size_t PileLen(struct Pile *const self);
void PilePushCard(struct Pile *const self, struct Card* c);
struct Card* PilePopCard(struct Pile *const self);
struct Card* PilePeekCard(struct Pile *const self);
struct CardAndIndex PileFindCard(struct Pile *const self, enum CardOrdinal ord, enum CardSuit suit);
struct Card* PileFindLastCardUnderPoint(struct Pile *const self, Vector2 pt);
_Bool PileIsStock(struct Pile *const self);
Rectangle PileBaizeRect(struct Pile *const self);
Rectangle PileScreenRect(struct Pile *const self);
Vector2 PileBaizePos(struct Pile *const self);
Vector2 PileScreenPos(struct Pile *const self);
Vector2 PileCalculatePosFromSlot(struct Pile *const self);
Rectangle PileFannedBaizeRect(struct Pile *const self);
Rectangle PileFannedScreenRect(struct Pile *const self);
Vector2 PilePushedFannedPos(struct Pile *const self);
_Bool PileMoveCard(struct Pile *const self, struct Pile *const src);
_Bool PileMoveCards(struct Pile *const self, struct Card const* c);
void PileRepushAllCards(struct Pile *const self);
void PileGenericTapped(struct Pile *const self, struct Array *const tail);
int PileGenericCollect(struct Pile *const self);
void PileUpdate(struct Pile *const self);
void PileDraw(struct Pile *const self);
void PileFree(struct Pile *const self);

#endif
