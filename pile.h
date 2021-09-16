/* pile.h */

#ifndef PILE_H
#define PILE_H

#include <raylib.h>
#include <lua.h>

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
    unsigned magic;
    struct PileVtable *vtable;
    struct Baize* owner;
    char category[16];
    enum FanType fan;
    Vector2 pos;
    char buildfunc[16];
    char dragfunc[16];
    struct Array* cards;
};

struct PileVtable {
    void (*CardTapped)(lua_State *L, struct Card *c);
    void (*PileTapped)(lua_State *L, struct Pile *p);
    bool (*CanAcceptTail)(struct Pile *const self, lua_State *L, struct Array *const tail);
    void (*SetAccept)(struct Pile *const self, enum CardOrdinal ord);

    void (*Update)(struct Pile *const self);
    void (*Draw)(struct Pile *const self);
    void (*Free)(struct Pile *const self);
};

void PileCtor(struct Pile *const self, const char* category, Vector2 pos, enum FanType fan, const char* buildfunc, const char* dragfunc);
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
bool PileIsAt(struct Pile *const self, Vector2 point);
void PileUpdate(struct Pile *const self);
void PileDraw(struct Pile *const self);
void PileFree(struct Pile *const self);

#endif
