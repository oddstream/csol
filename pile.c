/* pile.c */

#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include "pile.h"
#include "array.h"

struct Pile* PileNew(const char* class, Vector2 pos, enum FanType fan) {
    struct Pile* self = malloc(sizeof(struct Pile));
    if ( self ) {
        strncpy(self->class, class, sizeof self->class - 1);
        self->pos = pos;
        self->fan = fan;
        self->cards = ArrayNew(52);
    }
    return self;
}

void PilePush(struct Pile* self, struct Card* c) {
    CardSetOwner(c, self);
    CardSetPosition(c, self->pos);
    ArrayPush(&self->cards, (void**)c);
}

struct Card* PilePop(struct Pile* self) {
    struct Card* c = (struct Card*)ArrayPop(&self->cards);
    CardSetOwner(c, NULL);
    return c;
}

struct Card* PilePeek(struct Pile* self) {
    return (struct Card*)ArrayPeek(&self->cards);
}

Vector2 PileGetPosition(struct Pile* self) {
    return self->pos;
}

void PileUpdate(struct Pile* self) {
    (void)self;
}

void PileDraw(struct Pile* self) {
    (void)self;
}

void PileFree(struct Pile* self) {
    // Card objects exist in the Baize->cardLibrary array, so we don't free them here
    if ( self ) {
        ArrayFree(&self->cards);
        free(self);
    }
}
