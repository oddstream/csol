/* pile.c */

#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include "pile.h"
#include "array.h"

#define MAGIC (0xdeadbeef)

struct Pile* PileNew(const char* class, Vector2 pos, enum FanType fan) {
    struct Pile* self = malloc(sizeof(struct Pile));
    if ( self ) {
        self->magic = MAGIC;
        strncpy(self->class, class, sizeof self->class - 1);
        self->pos = pos;
        self->fan = fan;
        self->cards = ArrayNew(52);
    }
    return self;
}

bool PileValid(struct Pile* self) {
    return self && self->magic == MAGIC;
}

size_t PileLen(struct Pile* self) {
    return ArrayLen(self->cards);
}

void PilePush(struct Pile* self, struct Card* c) {
    CardSetOwner(c, self);
    CardSetPosition(c, self->pos);
    ArrayPush(self->cards, (void**)c);
}

struct Card* PilePop(struct Pile* self) {
    struct Card* c = (struct Card*)ArrayPop(self->cards);
    if ( c ) {
        CardSetOwner(c, NULL);
    }
    return c;
}

struct Card* PilePeek(struct Pile* self) {
    return (struct Card*)ArrayPeek(self->cards);
}

Vector2 PileGetPosition(struct Pile* self) {
    return self->pos;
}

void PileUpdate(struct Pile* self) {
    (void)self;
}

void PileDraw(struct Pile* self) {
    extern float cardWidth, cardHeight;
    // BeginDrawing() has been called by BaizeDraw()
    Rectangle r = {.x=self->pos.x, .y=self->pos.y, cardWidth, cardHeight};
    DrawRectangleRoundedLines(r, 0.1, 4, 2.0, (Color){255,255,255,31});
}

void PileFree(struct Pile* self) {
    // Card objects exist in the Baize->cardLibrary array, so we don't free them here
    if ( self ) {
        ArrayFree(self->cards);
        self->magic = 0;
        free(self);
    }
}
