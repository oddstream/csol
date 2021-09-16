/* card.c */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "raylib.h"
#include "spritesheet.h"
#include "card.h"
#include "util.h"

#define MAGIC (0x29041962)

extern struct Spritesheet *ssFace, *ssBack;

struct Card CardNew(enum CardOrdinal ord, enum CardSuit suit)
{
    struct Card self = {.magic = MAGIC, .ord = ord, .suit = suit, .prone = true, .dragging = false};
    self.frame = (suit * 13) + (ord - 1);   // TODO specific to retro spritesheet
    self.pos = (Vector2){0};
    self.flipWidth = 1.0f;
    self.flipStep = 0.0f;
    return self;    // return whole struct by value, gets copied into cardLibrary[]
}

bool CardValid(struct Card *const self)
{
    return self && self->magic == MAGIC;
}

void CardShorthand(struct Card *const self, char* z)
{
    static char ord[] = "0A23456789XJQK";
    static char suit[] = "CHDS";

    z[0] = ord[self->ord];
    z[1] = suit[self->suit];
    z[2] = '\0';
}

struct Pile* CardGetOwner(struct Card *const self)
{
    return self->owner;
}

void CardSetOwner(struct Card *const self, struct Pile* p)
{
    self->owner = p;
}

Rectangle CardGetRect(struct Card *const self)
{
    extern float cardWidth, cardHeight;
    return (Rectangle){.x = self->pos.x, .y = self->pos.y, .width = cardWidth, .height = cardHeight};
}

Vector2 CardGetPos(struct Card *const self)
{
    return self->pos;
}

void CardSetPos(struct Card *const self, Vector2 pos)
{
    self->pos = pos;
}

void CardMovePositionBy(struct Card *const self, Vector2 delta)
{
    self->pos.x += delta.x;
    self->pos.y += delta.y;
}

void CardTransitionTo(struct Card *const self, Vector2 pos)
{
    if ( pos.x == self->pos.x && pos.y == self->pos.y ) {
        self->lerpStep = 1.0f;    // stop any current lerp
        return;
    }
    self->lerpSrc = self->pos;
    self->lerpDst = pos;
    float dist = UtilDistance(self->lerpSrc, self->lerpDst);
    self->lerpStepAmount = fminf(0.025f, 15.0f*(1.0f/dist));
    self->lerpStep = 0.0;       // trigger a lerp
}

bool CardTransitioning(struct Card *const self)
{
    return self->lerpStep < 1.0f;
}

void CardStartDrag(struct Card *const self)
{
    self->dragStartPos = self->pos;
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

bool CardIsAt(struct Card *const self, Vector2 point)
{
    extern float cardWidth, cardHeight;
    Rectangle rect = {.x=self->pos.x, .y=self->pos.y, .width=cardWidth, .height=cardHeight};
    return CheckCollisionPointRec(point, rect);
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

    if ( showFace ) {
        SpritesheetDraw(ssFace, self->frame, self->flipWidth, self->pos);
    } else {
        SpritesheetDraw(ssBack, 6, self->flipWidth, self->pos);
    }
}

// void CardDrawRect(struct Card *const self, int x, int y)
// {
//     extern float cardWidth, cardHeight;
//     char z[4], buf[104];
//     CardShorthand(self, z);
//     sprintf(buf, "%s %f,%f,%f,%f", z, self->pos.x, self->pos.y, cardWidth, cardHeight);
//     DrawText(buf, x, y, 24, WHITE);
// }

void CardFlipUp(struct Card *const self)
{
    if ( self->prone ) {
        self->prone = false;
        self->flipStep = -FLIPSTEPAMOUNT;   // start by making card narrower
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
