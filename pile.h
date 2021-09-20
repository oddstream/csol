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

enum DragType {
    DRAG_NONE = 0,
    DRAG_SINGLE,
    DRAG_SINGLEORPILE,
    DRAG_MANY,
};

struct PileVtable;  // forward declaration

struct Pile {
    unsigned magic;
    struct PileVtable *vtable;
    struct Baize *owner;
    char category[16];
    Vector2 slot;
    Vector2 pos;
    enum FanType fanType;
    enum DragType dragType;
    char buildfunc[16];
    char dragfunc[16];
    struct Array *cards;
};

struct PileVtable {
    bool (*CardTapped)(struct Card *c);
    bool (*PileTapped)(struct Pile *p);
    bool (*CanAcceptTail)(struct Pile *const self, lua_State *L, struct Array *const tail);
    void (*SetAccept)(struct Pile *const self, enum CardOrdinal ord);
    void (*SetRecycles)(struct Pile *const self, int r);

    void (*Update)(struct Pile *const self);
    void (*Draw)(struct Pile *const self);
    void (*Free)(struct Pile *const self);
};

void PileCtor(struct Pile *const self, const char* category, Vector2 slot, enum FanType fan, enum DragType drag, const char* buildfunc, const char* dragfunc);
bool PileValid(struct Pile *const self);
size_t PileLen(struct Pile *const self);
void PilePushCard(struct Pile *const self, struct Card* c);
struct Card* PilePopCard(struct Pile *const self);
struct Card* PilePeekCard(struct Pile *const self);
bool PileIsStock(struct Pile *const self);
Rectangle PileGetBaizeRect(struct Pile *const self);
Vector2 PileGetBaizePos(struct Pile *const self);
Vector2 PileGetScreenPos(struct Pile *const self);
Vector2 PileCalculatePosFromSlot(struct Pile *const self);
Rectangle PileGetFannedBaizeRect(struct Pile *const self);
Rectangle PileGetFannedScreenRect(struct Pile *const self);
Vector2 PileGetPushedFannedPos(struct Pile *const self);
bool PileMoveCards(struct Pile *const self, struct Card* c);
bool PileIsAt(struct Pile *const self, Vector2 point);
void PileRepushAllCards(struct Pile *const self);
void PileUpdate(struct Pile *const self);
void PileDraw(struct Pile *const self);
void PileFree(struct Pile *const self);

#endif
