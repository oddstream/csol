/* card.c */

#include <stdlib.h>
#include <math.h>

#include "raylib.h"
#include "spritesheet.h"
#include "card.h"
#include "util.h"

extern struct Spritesheet *ssFace, *ssBack;

struct Card CardNew(enum CardOrdinal ord, enum CardSuit suit) {
    struct Card self = {.ord = ord, .suit = suit, .prone = true, .dragging = false};
    self.frame = (suit * 13) + (ord - 1);
    self.baizePos = (Vector2){0};
    return self;
}

void CardShorthand(struct Card *const self, char* z) {
    static char ord[] = "0A23456789XJQK";
    static char suit[] = "CHDS";

    z[0] = ord[self->ord];
    z[1] = suit[self->suit];
    z[2] = '\0';
}

void CardSetOwner(struct Card *const self, struct Pile* p) {
    self->owner = p;
}

struct Pile* CardGetOwner(struct Card *const self) {
    return self->owner;
}

void CardSetPosition(struct Card *const self, Vector2 pos) {
    self->baizePos = pos;
}

void CardMovePositionBy(struct Card *const self, Vector2 delta) {
    self->baizePos.x += delta.x;
    self->baizePos.y += delta.y;
}

void CardTransitionTo(struct Card *const self, Vector2 pos) {
    if ( pos.x == self->baizePos.x && pos.y == self->baizePos.y ) {
        self->lerpStep = 1.0;    // stop any current lerp
        return;
    }
    self->lerpSrc = self->baizePos;
    self->lerpDst = pos;
    float dist = UtilDistance(self->lerpSrc, self->lerpDst);
    self->lerpStepAmount = fminf(0.025, 15.0*(1.0/dist));
    self->lerpStep = 0.0;       // trigger a lerp
}

bool CardTransitioning(struct Card *const self) {
    return self->lerpStep < 1.0;
}

void CardStartDrag(struct Card *const self) {
    self->dragStartPos = self->baizePos;
    self->dragging = true;
}

void CardStopDrag(struct Card *const self) {
    self->dragging = false;
    CardTransitionTo(self, self->dragStartPos);
}

bool CardNotDragged(struct Card *const self) {
    return self->baizePos.x == self->dragStartPos.x && self->baizePos.y == self->dragStartPos.y;
}

bool CardDragging(struct Card *const self) {
    return self->dragging;
}

bool CardIsAt(struct Card *const self, Vector2 point) {
    extern float cardWidth, cardHeight;
    Rectangle rect = {.x=self->baizePos.x, .y=self->baizePos.y, .width=cardWidth, .height=cardHeight};
    return CheckCollisionPointRec(point, rect);
}

void CardUpdate(struct Card *const self) {

    if ( self->lerpStep < 1.0 ) {
        self->baizePos.x = UtilSmootherstep(self->lerpSrc.x, self->lerpDst.x, self->lerpStep);
        self->baizePos.y = UtilSmootherstep(self->lerpSrc.y, self->lerpDst.y, self->lerpStep);
        self->lerpStep += self->lerpStepAmount;
        if ( self->lerpStep >= 1.0 ) {
            self->baizePos = self->lerpDst;
        }
    }
}

void CardDraw(struct Card *const self) {
    // BeginDrawing() has been called by BaizeDraw()
    if ( self->prone ) {
        SpritesheetDraw(ssBack, 6, self->baizePos);
    } else {
        SpritesheetDraw(ssFace, self->frame, self->baizePos);
    }
}

void CardFlipUp(struct Card *const self) {
    self->prone = false;
}

void CardFlipDown(struct Card *const self) {
    self->prone = true;
}

void CardFree(struct Card *const self) {
    (void)self;
}
