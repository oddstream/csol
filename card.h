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
    int frame;
    enum CardSuit suit;
    enum CardOrdinal ord;
    bool prone;
    Vector2 baizePos;
    struct Pile* owner;
} /* variable definition */;

struct Card CardNew(enum CardSuit suit, enum CardOrdinal ord);
void CardShorthand(struct Card* self, char* z);
void CardSetPosition(struct Card* self, Vector2 pos);
void CardSetOwner(struct Card* self, struct Pile* p);
struct Pile* CardGetOwner(struct Card* self);
bool CardIsAt(struct Card* c, Vector2 point);
void CardFlipUp(struct Card* self);
void CardFlipDown(struct Card*self);
void CardUpdate(struct Card* self);
void CardDraw(struct Card* self);
void CardFree(struct Card* self);

#endif
