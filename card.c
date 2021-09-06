/* Card.c */

#include <stdlib.h>

#include "raylib.h"
#include "spritesheet.h"
#include "card.h"

extern struct Spritesheet* ssFace;
extern struct Spritesheet* ssBack;

struct Card CardNew(enum CardSuit suit, enum CardOrdinal ord) {
    struct Card self = {.suit = suit, .ord = ord, .prone = false};
    // c.s = s;
    // c.suit = suit;
    // c.ord = ord;
    // c.prone = false;
    self.frame = (suit * 13) + (ord - 1);
    self.rect.x = self.rect.y = 0.0;
    self.rect.width = 71.0;
    self.rect.height = 96.0;
    return self;
}

void CardSetOwner(struct Card* self, struct Pile* p) {
    self->owner = p;
}

struct Pile* CardgetOwner(struct Card* self) {
    return self->owner;
}

void CardSetPosition(struct Card* self, Vector2 pos) {
    self->rect.x = pos.x;
    self->rect.y = pos.y;
}

bool CardIsAt(struct Card* self, Vector2 point) {
    return CheckCollisionPointRec(point, self->rect);
}

void CardDraw(struct Card* self) {
    if ( self->prone ) {
        SpritesheetDraw(ssBack, 6, self->rect.x, self->rect.y);
    } else {
        SpritesheetDraw(ssFace, self->frame, self->rect.x, self->rect.y);
    }
}

void CardFlip(struct Card* self) {
    self->prone = !self->prone;
}

void CardFree(struct Card* self) {
    (void)self;
}