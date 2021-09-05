/* Card.c */

#include <stdlib.h>

#include "raylib.h"
#include "spritesheet.h"
#include "card.h"

struct Card CardNew(struct Spritesheet* ssFace, struct Spritesheet* ssBack, enum CardSuit suit, enum CardOrdinal ord) {
    struct Card c = {.ssFace = ssFace, .ssBack = ssBack, .suit = suit, .ord = ord, .prone = false};
    // c.s = s;
    // c.suit = suit;
    // c.ord = ord;
    // c.prone = false;
    c.frame = (suit * 13) + (ord - 1);
    c.rect.x = c.rect.y = 0.0;
    c.rect.width = 71.0;
    c.rect.height = 96.0;
    return c;
}

void CardSetPosition(struct Card* c, Vector2 pos) {
    c->rect.x = pos.x;
    c->rect.y = pos.y;
}

bool CardIsAt(struct Card* c, Vector2 point) {
    return CheckCollisionPointRec(point, c->rect);
}

void CardDraw(struct Card* c) {
    if ( c->prone ) {
        SpritesheetDraw(c->ssBack, 6, c->rect.x, c->rect.y);
    } else {
        SpritesheetDraw(c->ssFace, c->frame, c->rect.x, c->rect.y);
    }
}

void CardFlip(struct Card* c) {
    c->prone = !c->prone;
}

void CardFree(struct Card* c) {
    (void)c;
}