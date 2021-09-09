/* pile.c */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <raylib.h>
#include "pile.h"
#include "array.h"

#define MAGIC (0xdeadbeef)

#define CARD_FACE_FAN_FACTOR ((float)3)
#define CARD_BACK_FAN_FACTOR ((float)5)

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
    Vector2 fannedPos = PilePushedFannedPosition(self); // do this *before* pushing card to pile
    CardSetPosition(c, fannedPos);
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

void PileShuffle(struct Pile* self) {
    // Knuth-Fisher–Yates shuffle
    srand(time(NULL));
    int n = ArrayLen(self->cards);
    for ( int i = n-1; i > 0; i-- ) {
        int j = rand() % (i+1);
        ArraySwap(self->cards, i, j);
    }
}

Vector2 PilePushedFannedPosition(struct Pile* self) {
    extern float cardWidth, cardHeight;
    Vector2 pos = self->pos;
    float faceDelta, backDelta;
    struct Card* c;
    switch ( self->fan ) {
        case NONE:
            // do nothing
            break;
        case RIGHT:
            faceDelta = cardWidth / CARD_FACE_FAN_FACTOR;
            backDelta = cardWidth / CARD_BACK_FAN_FACTOR;
            c = (struct Card*)ArrayFirst(self->cards);
            while ( c ) {
                pos.x += c->prone ? backDelta : faceDelta;
                c = (struct Card*)ArrayNext(self->cards);
            }
            break;
        case LEFT:
            faceDelta = cardWidth / CARD_FACE_FAN_FACTOR;
            backDelta = cardWidth / CARD_BACK_FAN_FACTOR;
            c = (struct Card*)ArrayFirst(self->cards);
            while ( c ) {
                pos.x -= c->prone ? backDelta : faceDelta;
                c = (struct Card*)ArrayNext(self->cards);
            }
            break;
        case DOWN:
            faceDelta = cardHeight / CARD_FACE_FAN_FACTOR;
            backDelta = cardHeight / CARD_BACK_FAN_FACTOR;
            c = (struct Card*)ArrayFirst(self->cards);
            while ( c ) {
                pos.y += c->prone ? backDelta : faceDelta;
                c = (struct Card*)ArrayNext(self->cards);
            }
            break;
        case WASTE_RIGHT:
            break;
        case WASTE_LEFT:
            break;
        case WASTE_DOWN:
            break;
    }
    return pos;
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
