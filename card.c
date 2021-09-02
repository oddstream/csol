/* card.c */

#include <stdlib.h>

#include "raylib.h"
#include "spritesheet.h"
#include "card.h"

struct card {
    struct spritesheet *s;
    int frame;
    enum CardSuit suit;
    enum CardOrdinal ord;
    Rectangle rect;
};

struct card* card_new(struct spritesheet* s, enum CardSuit suit, enum CardOrdinal ord) {
    struct card* c = malloc(sizeof(struct card));
    if ( NULL == c ) {
        return NULL;
    }
    c->s = s;
    c->suit = suit;
    c->ord = ord;
    c->frame = (suit * 13) + (ord - 1);
    c->rect.x = c->rect.y = 0;
    c->rect.width = c->rect.height = 0;
    return c;
}

void card_dispose(struct card* c) {
    free(c);
}

void card_position(struct card* c, int x, int y) {
    c->rect.x = x;
    c->rect.y = y;
}

void card_draw(struct card* c) {
    spritesheet_draw(c->s, c->frame, (float)c->rect.x, (float)c->rect.y);
}
