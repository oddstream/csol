/* Card.c */

#include <stdlib.h>

#include "raylib.h"
#include "spritesheet.h"
#include "card.h"

extern struct Spritesheet* ssFace;
extern struct Spritesheet* ssBack;

struct Card CardNew(enum CardSuit suit, enum CardOrdinal ord) {
    struct Card self = {.suit = suit, .ord = ord, .prone = true};
    self.frame = (suit * 13) + (ord - 1);
    self.baizePos = (Vector2){0};
    return self;
}

void CardShorthand(struct Card* self, char* z) {
    static char suit[] = "CHDS";
    static char ord[] = "0A23456789XJQK";

    z[0] = suit[self->suit];
    z[1] = ord[self->ord];
    z[2] = '\0';
}

void CardSetOwner(struct Card* self, struct Pile* p) {
    self->owner = p;
}

struct Pile* CardGetOwner(struct Card* self) {
    return self->owner;
}

void CardSetPosition(struct Card* self, Vector2 pos) {
    // sets the baize position of this card
    self->baizePos = pos;
}

bool CardIsAt(struct Card* self, Vector2 point) {
    extern float cardWidth, cardHeight;
    Rectangle rect = {.x=self->baizePos.x, .y=self->baizePos.y, .width=cardWidth, .height=cardHeight};
    return CheckCollisionPointRec(point, rect);
}

void CardUpdate(struct Card* self) {
    (void)self;
}

void CardDraw(struct Card* self) {
    // BeginDrawing() has been called by BaizeDraw()
    if ( self->prone ) {
        SpritesheetDraw(ssBack, 6, self->baizePos);
    } else {
        SpritesheetDraw(ssFace, self->frame, self->baizePos);
    }
}

void CardFlipUp(struct Card* self) {
    self->prone = false;
}

void CardFlipDown(struct Card* self) {
    self->prone = true;
}

void CardFree(struct Card* self) {
    (void)self;
}