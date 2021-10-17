/* Card.h */

#ifndef CARD_H
#define CARD_H

#include <stdlib.h>

enum CardOrdinal {
    ACE = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE = 9,
    TEN = 10,
    JACK = 11,
    QUEEN = 12,
    KING = 13,
};

enum CardSuit {
    CLUB = 0,
    DIAMOND = 1,
    HEART = 2,
    SPADE = 3,
};

struct CardId {
    unsigned int pack:4;
    unsigned int ordinal:4;
    unsigned int suit:4;
};

struct Card /* tag */ {
    unsigned magic;
    struct CardId id;
    bool prone;
    Vector2 pos;
    struct Pile* owner;

    int frame;  // spritesheet frame, index into Info Vector2D struct

    bool dragging;
    Vector2 dragStartPos;
    Vector2 lerpSrc, lerpDst;
    float lerpStep, lerpStepAmount;

    float flipStep, flipWidth;
} /* variable definition */;

#define FLIPSTEPAMOUNT (0.075f)
#define LOGCARD(c)              { char z[64]; CardToString(c, z); fprintf(stdout, "%s\n", z); }

struct Card CardNew(unsigned pack, enum CardOrdinal ord, enum CardSuit suit);
bool CardValid(struct Card *const self);
void CardToString(struct Card *const self, char* z);
Vector2 CardBaizePos(struct Card *const self);
Vector2 CardScreenPos(struct Card *const self);
Rectangle CardBaizeRect(struct Card *const self);
Rectangle CardScreenRect(struct Card *const self);
void CardMovePositionBy(struct Card *const self, Vector2 delta);
void CardTransitionTo(struct Card *const self, Vector2 pos);
bool CardTransitioning(struct Card *const self);
void CardStartDrag(struct Card *const self);
void CardStopDrag(struct Card *const self);
void CardCancelDrag(struct Card *const self);
bool CardWasDragged(struct Card *const self);
bool CardDragging(struct Card *const self);
void CardFlipUp(struct Card *const self);
void CardFlipDown(struct Card *const self);
bool CardFlipping(struct Card *const self);
void CardUpdate(struct Card *const self);
void CardDraw(struct Card *const self);
void CardFree(struct Card *const self);
// void CardDrawRect(struct Card *const self, int x, int y);

#endif
