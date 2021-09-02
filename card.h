/* card.h */

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

struct card;

struct card* card_new(struct spritesheet* s, enum CardSuit suit, enum CardOrdinal ord);
void card_dispose(struct card*);
void card_position(struct card* c, int x, int y);
void card_draw(struct card*);