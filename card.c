/* card.c */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <raylib.h>

#include "baize.h"
#include "pile.h"
#include "spritesheet.h"
#include "card.h"
#include "util.h"

#define CARD_MAGIC (0x29041962)

extern struct Spritesheet *ssFace, *ssBack;

struct Card CardNew(unsigned pack, enum CardOrdinal ord, enum CardSuit suit)
{
    struct Card self = {.magic = CARD_MAGIC, .owner = NULL, .id.pack = pack, .id.ordinal = ord, .id.suit = suit, .prone = 1, .dragging = false};
    self.frame = (suit * 13) + (ord - 1);   // TODO specific to retro spritesheet
    self.pos = (Vector2){.x=0.0f,.y=0.0f};
    self.flipWidth = 1.0f;
    self.flipStep = 0.0f;
    self.dragStartPos = self.pos;
    self.lerpSrc = self.pos;
    self.lerpDst = self.pos;
    return self;    // return whole struct by value, gets saved into cardLibrary[]
}

bool CardValid(struct Card *const self)
{
    return self && self->magic == CARD_MAGIC;
}

void CardToString(struct Card *const self, char* z)
{
    unsigned dw = *((unsigned*)(&self->id));
    sprintf(z, "{%x: p=%u o=%s s=%s p=%u}", dw, self->id.pack, UtilOrdToShortString(self->id.ordinal), UtilSuitToShortString(self->id.suit), self->prone);
}

Vector2 CardBaizePos(struct Card *const self)
{
    return self->pos;
}

Vector2 CardScreenPos(struct Card *const self)
{
    return (Vector2){.x = self->pos.x + self->owner->owner->dragOffset.x, .y = self->pos.y + self->owner->owner->dragOffset.y};
}

Rectangle CardBaizeRect(struct Card *const self)
{
    extern float cardWidth, cardHeight;
    return (Rectangle){.x = self->pos.x, .y = self->pos.y, .width = cardWidth, .height = cardHeight};
}

Rectangle CardScreenRect(struct Card *const self)
{
    extern float cardWidth, cardHeight;
    Vector2 csp = CardScreenPos(self);
    return (Rectangle){.x=csp.x, .y=csp.y, .width=cardWidth, .height=cardHeight}; 
}

void CardMovePositionBy(struct Card *const self, Vector2 delta)
{
    // fprintf(stdout, "CardDragBy %.0f, %.0f\n", delta.x, delta.y);
    self->pos.x += delta.x;
    self->pos.y += delta.y;
}

void CardTransitionTo(struct Card *const self, Vector2 pos)
{
    // const float speed = 15.0f;
    if ( pos.x == self->pos.x && pos.y == self->pos.y ) {
        self->lerpStep = 1.0f;    // stop any current lerp
        return;
    }
    self->lerpSrc = self->pos;
    self->lerpDst = pos;
    // float dist = UtilDistance(self->lerpSrc, self->lerpDst);
    // self->lerpStepAmount = fminf(0.025f, speed*(1.0f/dist));
    self->lerpStepAmount = 0.025f;
    self->lerpStep = 0.0;       // trigger a lerp
}

bool CardTransitioning(struct Card *const self)
{
    return self->lerpStep < 1.0f;
}

void CardStartDrag(struct Card *const self)
{
    if ( !CardTransitioning(self) ) {
        self->dragStartPos = self->pos;
    }
    // if grabbing a transitioning card, the start position should be the original position, not the in-flight one
    self->dragging = true;
}

void CardStartDrag2(struct Card *const self, Vector2 pos)
{
    if ( !CardTransitioning(self) ) {
        self->dragStartPos = pos;
    }
    // if grabbing a transitioning card, the start position should be the original position, not the in-flight one
    self->dragging = true;
}

void CardStopDrag(struct Card *const self)
{
    self->dragging = false;
}

void CardCancelDrag(struct Card *const self)
{
    self->dragging = false;
    CardTransitionTo(self, self->dragStartPos);
}

bool CardWasDragged(struct Card *const self)
{
    return self->pos.x != self->dragStartPos.x || self->pos.y != self->dragStartPos.y;
}

bool CardDragging(struct Card *const self)
{
    return self->dragging;
}

void CardUpdate(struct Card *const self)
{
    if ( CardFlipping(self) ) {
        self->flipWidth += self->flipStep;
        if ( self->flipWidth <= 0.0f ) {
            self->flipStep = FLIPSTEPAMOUNT;    // now make card wider
        } else if ( self->flipWidth >= 1.0f ) {
            self->flipWidth = 1.0f;             // finished flipping
            self->flipStep = 0.0f;
        }
    }
    if ( CardTransitioning(self) ) {
        self->pos.x = UtilSmootherstep(self->lerpSrc.x, self->lerpDst.x, self->lerpStep);
        self->pos.y = UtilSmootherstep(self->lerpSrc.y, self->lerpDst.y, self->lerpStep);
        self->lerpStep += self->lerpStepAmount;
        if ( self->lerpStep >= 1.0f ) {
            self->pos = self->lerpDst;
        }
    }
}

void CardDraw(struct Card *const self)
{
    // BeginDrawing() has been called by BaizeDraw()

    bool showFace;

    // card prone has already been set to destination state
    if ( self->flipStep < 0.0f ) {
        if ( self->prone ) {
            // card is getting narrower, and it's going to show face down, but show face up
            showFace = true;
        } else {
            // card is getting narrower, and it's going to show face up, but show face down
            showFace = false;
        }
    } else {
        if ( self->prone ) {
            showFace = false;
        } else {
            showFace = true;
        }
    }

    Rectangle rectCard = CardScreenRect(self);

    if ( CardDragging(self) ) {
        rectCard.x += 2.0f;
        rectCard.y += 2.0f;
        DrawRectangleRounded(rectCard, 0.05, 9, (Color){0,0,0,63});
        rectCard.x -= 4.0f;
        rectCard.y -= 4.0f;
    }

    if ( showFace ) {
        SpritesheetDraw(ssFace, self->frame, self->flipWidth, rectCard);
    } else {
        SpritesheetDraw(ssBack, 2, self->flipWidth, rectCard);
    }
}

// void CardDrawRect(struct Card *const self, int x, int y)
// {
//     extern float cardWidth, cardHeight;
//     char z[64], buf[104];
//     CardToString(self, z);
//     sprintf(buf, "%s %f,%f,%f,%f", z, self->pos.x, self->pos.y, cardWidth, cardHeight);
//     DrawText(buf, x, y, 24, WHITE);
// }

void CardFlipUp(struct Card *const self)
{
    if ( self->prone ) {
        self->prone = false;
        self->flipStep = -FLIPSTEPAMOUNT;   // start by making card narrower ...
        self->flipWidth = 1.0f;             // ... from it's full width
    }
}

void CardFlipDown(struct Card *const self)
{
    if ( !self->prone ) {
        self->prone = true;
        self->flipStep = -FLIPSTEPAMOUNT;   // start by making card narrower ...
        self->flipWidth = 1.0f;             // ... from it's full width
    }
}

bool CardFlipping(struct Card *const self)
{
    return self->flipStep != 0.0f;
}

void CardFree(struct Card *const self)
{
    // struct Card doesn't contain any allocated objects, so not much to do here
    self->magic = 0;
}
