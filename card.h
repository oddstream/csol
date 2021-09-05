/* Card.h */

#ifndef CARD_H
#define CARD_H

enum CardSuit {
    CLUB = 0,
    DIAMOND = 1,
    HEART = 2,
    SPADE = 3,
};

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

struct Card /* tag */ {
    struct Spritesheet *ssFace;
    struct Spritesheet *ssBack;
    int frame;
    enum CardSuit suit;
    enum CardOrdinal ord;
    bool prone;
    Rectangle rect;
} /* variable definition */;

struct Card CardNew(struct Spritesheet* ssFace, struct Spritesheet* ssBack, enum CardSuit suit, enum CardOrdinal ord);
void CardSetPosition(struct Card* c, Vector2 pos);
bool CardIsAt(struct Card* c, Vector2 point);
void CardDraw(struct Card*);
void CardFlip(struct Card*);
void CardFree(struct Card*);

#endif
