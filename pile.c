/* pile.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include "pile.h"
#include "array.h"

#define MAGIC (0xdeadbeef)

#define CARD_FACE_FAN_FACTOR ((float)3)
#define CARD_BACK_FAN_FACTOR ((float)5)

// struct Pile* PileNew(const char* category, Vector2 pos, enum FanType fan) {
//     struct Pile* self = malloc(sizeof(struct Pile));
//     if ( self ) {
//         self->magic = MAGIC;
//         strncpy(self->category, category, sizeof self->category - 1);
//         self->pos = pos;
//         self->fan = fan;
//         self->cards = ArrayNew(52);
//     }
//     return self;
// }

void PileCtor(struct Pile *const self, const char* category, Vector2 pos, enum FanType fan) {
    self->magic = MAGIC;
    strncpy(self->category, category, sizeof self->category - 1);
    self->pos = pos;
    self->fan = fan;
    self->cards = ArrayNew(52);
}

bool PileValid(struct Pile *const self) {
    return self && self->magic == MAGIC;
}

size_t PileLen(struct Pile *const self) {
    return ArrayLen(self->cards);
}

void PilePushCard(struct Pile *const self, struct Card* c) {
    CardSetOwner(c, self);
    Vector2 fannedPos = PileGetPushedFannedPos(self); // do this *before* pushing card to pile
    CardTransitionTo(c, fannedPos);
    // CardSetPos(c, self->pos);
    ArrayPush(self->cards, c);
}

struct Card* PilePopCard(struct Pile *const self) {
    struct Card* c = (struct Card*)ArrayPop(self->cards);
    if ( c ) {
        CardSetOwner(c, NULL);
    }
    return c;
}

struct Card* PilePeekCard(struct Pile *const self) {
    return (struct Card*)ArrayPeek(self->cards);
}

Vector2 PileGetPos(struct Pile *const self) {
    return self->pos;
}

Rectangle PileGetRect(struct Pile *const self) {
    extern float cardWidth, cardHeight;
    return (Rectangle){.x = self->pos.x, .y = self->pos.y, .width = cardWidth, .height = cardHeight};
}

void PileSetPos(struct Pile *const self, Vector2 pos) {
    self->pos = pos;
}

Rectangle PileGetFannedRect(struct Pile *const self) {
    // cannot use position of top card, in case it's being dragged
    extern float cardWidth, cardHeight;
    Rectangle r = PileGetRect(self);
    if ( ArrayLen(self->cards) > 2 ) {
        struct Card* c = ArrayPeek(self->cards);
        Vector2 cPos = CardGetPos(c);
        switch ( self->fan ) {
            case NONE:
                // do nothing
                break;
            case RIGHT:
            case WASTE_RIGHT:
                r.width = cPos.x + cardWidth - r.x;
                break;
            case LEFT:
            case WASTE_LEFT:
                r.width = cPos.x - cardWidth - r.x;
                break;
            case DOWN:
            case WASTE_DOWN:
                r.height = cPos.y + cardHeight - r.y;
                break;
        }
    }
    return r;
}

Vector2 PileGetPushedFannedPos(struct Pile *const self) {
    extern float cardWidth, cardHeight;

    Vector2 pos = self->pos;
    float faceDelta, backDelta;
    struct Card* c;
    size_t index = 0;

    switch ( self->fan ) {
        case NONE:
            // do nothing
            break;
        case RIGHT:
            faceDelta = cardWidth / CARD_FACE_FAN_FACTOR;
            backDelta = cardWidth / CARD_BACK_FAN_FACTOR;
            c = (struct Card*)ArrayFirst(self->cards, &index);
            while ( c ) {
                pos.x += c->prone ? backDelta : faceDelta;
                c = (struct Card*)ArrayNext(self->cards, &index);
            }
            break;
        case LEFT:
            faceDelta = cardWidth / CARD_FACE_FAN_FACTOR;
            backDelta = cardWidth / CARD_BACK_FAN_FACTOR;
            c = (struct Card*)ArrayFirst(self->cards, &index);
            while ( c ) {
                pos.x -= c->prone ? backDelta : faceDelta;
                c = (struct Card*)ArrayNext(self->cards, &index);
            }
            break;
        case DOWN:
            faceDelta = cardHeight / CARD_FACE_FAN_FACTOR;
            backDelta = cardHeight / CARD_BACK_FAN_FACTOR;
            c = (struct Card*)ArrayFirst(self->cards, &index);
            while ( c ) {
                pos.y += c->prone ? backDelta : faceDelta;
                c = (struct Card*)ArrayNext(self->cards, &index);
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

void PileMoveCards(struct Pile *const self, struct Card* c) {
    // move cards to this pile

    struct Pile* src = CardGetOwner(c);
    size_t oldSrcLen = PileLen(src);

    // find the new length of the source pile
    size_t newSrcLen = 0, index;
    struct Card* pc = ArrayFirst(src->cards, &index);
    while ( pc ) {
        if ( pc == c ) {
            break;
        }
        newSrcLen++;
        pc = ArrayNext(src->cards, &index);
    }

    // pop the tail off the source and push onto tmp stack
    struct Array* tmp = ArrayNew(PileLen(self) + PileLen(src));
    while ( PileLen(src) != newSrcLen ) {
        ArrayPush(tmp, PilePopCard(src));
    }

    // make some noise

    // pop all cards off the tmp stack and onto the destination (self)
    while ( ArrayLen(tmp) ) {
        PilePushCard(self, ArrayPop(tmp));
    }
    ArrayFree(tmp);

    // if ( newSrcLen != oldSrcLen ) {
    //     fprintf(stderr, "Something went wrong moving cards from %s to %s\n", src->category, self->category);
    // }
    fprintf(stderr, "old %lu, new %lu\n", oldSrcLen, newSrcLen);

    // flip up an exposed source card

    // special case: waste may need refanning if we took a card from it

    // scrunch
}

void PileUpdate(struct Pile *const self) {
    ArrayForeach(self->cards, (ArrayIterFunc)CardUpdate);
}

void PileDraw(struct Pile *const self) {
    // BeginDrawing() has been called by BaizeDraw()
    Rectangle r = PileGetFannedRect(self);
    DrawRectangleRoundedLines(r, 0.1, 4, 2.0, (Color){255,255,255,31});
}

void PileFree(struct Pile *const self) {
    // Card objects exist in the Baize->cardLibrary array, so we don't free them here
    if ( self ) {
        ArrayFree(self->cards);
        self->magic = 0;
        free(self);
    }
}
