/* pile.h */

#pragma once

#ifndef PILE_H
#define PILE_H

#include <raylib.h>

#include "array.h"
#include "card.h"

#define MAX_PILE_LABEL (31)

#define MIN_FAN_FACTOR (3.0f)
#define MAX_FAN_FACTOR (7.0f)

#define CARD_FACE_FAN_FACTOR_V (3.0f)
#define CARD_FACE_FAN_FACTOR_H (4.0f)
#define CARD_BACK_FAN_FACTOR (8.0f)

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
    Vector2 pos1;   // waste pos
    Vector2 pos2;   // waste pos
    enum FanType fanType;
    float fanFactor;    // all non-waste fan types, both horz and vert
    float defaultFanFactor;
    Vector2 scrunchDims;
    char label[MAX_PILE_LABEL + 1];
    struct Array *cards;
};

struct PileVtable {
    _Bool (*CanMoveTail)(struct Array *const tail);
    _Bool (*CanAcceptCard)(struct Baize *const baize, struct Pile *const self, struct Card *const c);
    _Bool (*CanAcceptTail)(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
    void (*PileTapped)(struct Pile *const self);
    void (*TailTapped)(struct Pile *const self, struct Array *const tail);
    int (*Collect)(struct Pile *p);
    _Bool (*Complete)(struct Pile *p);
    void (*SetRecycles)(struct Pile *const self, int r);
    int (*UnsortedPairs)(struct Pile *const self);
    void (*Reset)(struct Pile *const self);
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

void PileSetLabel(struct Pile *const self, const char* label);

void PilePushCard(struct Pile *const self, struct Card* c);
struct Card* PilePopCard(struct Pile *const self);
struct Card* PilePeekCard(struct Pile *const self);

struct CardAndIndex PileFindCard(struct Pile *const self, enum CardOrdinal ord, enum CardSuit suit);
// struct Card* PileFindLastCardUnderPoint(struct Pile *const self, Vector2 pt);
_Bool PileIsStock(struct Pile *const self);

void PileSetBaizePos(struct Pile *const self, Vector2 pos);
Vector2 PileBaizePos(struct Pile *const self);
Vector2 PileScreenPos(struct Pile *const self);

Rectangle PileBaizeRect(struct Pile *const self);
Rectangle PileScreenRect(struct Pile *const self);

// void PileDrawUpperLeftGlyph(struct Pile *const self, int glyph);
void PileDrawCenteredGlyph(struct Pile *const self, int glyph);
// void PileDrawUpperLeftText(struct Pile *const self, const char *text);
void PileDrawCenteredText(struct Pile *const self, const char *text);
Rectangle PileFannedBaizeRect(struct Pile *const self);
Rectangle PileFannedScreenRect(struct Pile *const self);
Vector2 PilePosAfter(struct Pile *const self, struct Card *const c);
void PileRefan(struct Pile *const self);
_Bool PileMoveCard(struct Pile *const self, struct Pile *const src);
_Bool PileMoveCards(struct Pile *const self, struct Card const* c);

_Bool PileInertCanMoveTail(struct Array *const Tail);
_Bool PileInertCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c);
_Bool PileInertCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail);
void PileInertPileTapped(struct Pile *const self);
void PileInertTailTapped(struct Pile *const self, struct Array *const tail);
void PileGenericTailTapped(struct Pile *const self, struct Array *const tail);
int PileInertCollect(struct Pile *const self);
int PileGenericCollect(struct Pile *const self);
_Bool PileInertComplete(struct Pile *const self);
void PileInertSetRecycles(struct Pile *const self, int r);
int PileGenericUnsortedPairs(struct Pile *const self);
int PileInertUnsortedPairs(struct Pile *const self);

void PileReset(struct Pile *const self);
void PileUpdate(struct Pile *const self);
void PileDraw(struct Pile *const self);
void PileFree(struct Pile *const self);

#endif
