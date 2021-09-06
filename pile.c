/* pile.c */

#include <stdlib.h>
#include <raylib.h>
#include "pile.h"
#include "array.h"

struct Pile* PileNew(Vector2 pos) {
    struct Pile* self = malloc(sizeof(struct Pile));
    self->pos = pos;
    self->cards = ArrayNew(52);
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
    // Card objects exists in the cardLibrary array
    // for ( int i = 0; i<ArrayLen(&p->cards); i++ ) {
    //     struct Card* pc = ArrayGet(&p->cards, i);
    //     CardFree(pc);
    // }
    ArrayFree(&self->cards);
    free(self);
}
