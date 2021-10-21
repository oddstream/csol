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
#define DEBUG_SPEED  (0.0025f)
#define SLOW_SPEED (0.010f)
#define NORMAL_SPEED (0.02f)
#define FAST_SPEED (0.040f)

extern struct Spritesheet *ssFace, *ssBack;

struct Card CardNew(unsigned pack, enum CardOrdinal ord, enum CardSuit suit)
{
    struct Card self = {.magic = CARD_MAGIC, .owner = NULL, .id.pack = pack, .id.ordinal = ord, .id.suit = suit, .prone = 1, .dragging = false};
    self.frame = (suit * 13) + (ord - 1); // index into Vector2D info struct, or retro spritesheet
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

struct Baize *CardToBaize(struct Card *const self)
{
    return self->owner->owner;
}

struct Pile *CardToPile(struct Card *const self)
{
    return self->owner;
}

unsigned CardToUnsigned(struct Card *const self)
{
    // error: dereferencing type-punned pointer will break strict-aliasing rules [-Werror=strict-aliasing]
    // return *((unsigned*)(&self->id));
    // also, this makes strings that usually end in 0, which is a waste of space

    // so, write stupid readable code and let the complier optimize it
    unsigned u;
    u = self->id.pack << 8; // put less-used pack in high bits to make serialized version shorter
    u |= self->id.ordinal << 4;
    u |= self->id.suit;
    return u;
}

struct CardId UnsignedToCardId(unsigned u)
{
    struct CardId id;
    id.pack = (u & /*0b111100000000*/0xf00) >> 8;
    id.ordinal = (u & /*0b11110000*/0xf0) >> 4;
    id.suit = u & /*0b1111*/0xf;
    return id;
}

void CardToString(struct Card *const self, char* z)
{
    sprintf(z, "{%x: p=%d o=%s s=%s p=%d}", CardToUnsigned(self), self->id.pack, UtilOrdToShortString(self->id.ordinal), UtilSuitToShortString(self->id.suit), self->prone);
}

void CardToShortString(struct Card *const self, char* z)
{
    z[0] = *UtilOrdToShortString(self->id.ordinal);
    z[1] = *UtilSuitToShortString(self->id.suit);
    z[2] = '\0';
}

Vector2 CardBaizePos(struct Card *const self)
{
    return self->pos;
}

Vector2 CardScreenPos(struct Card *const self)
{
    struct Baize* baize = CardToBaize(self);
    return (Vector2){.x = self->pos.x + baize->dragOffset.x, .y = self->pos.y + baize->dragOffset.y};
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
    if ( pos.x == self->pos.x && pos.y == self->pos.y ) {
        self->lerpStep = 1.0f;    // stop any current lerp
        return;
    }
    self->lerpSrc = self->pos;
    self->lerpDst = pos;
#if 1
    self->lerpStepAmount = NORMAL_SPEED;
#else
    // doing this makes the transition drawing order look wrong and ugly
    const float speed = 5.0f;
    float dist = UtilDistance(self->lerpSrc, self->lerpDst);
    self->lerpStepAmount = fminf(0.025f, speed*(1.0f/dist));
#endif
    self->lerpStep = 0.0f;       // trigger a lerp
}

bool CardTransitioning(struct Card *const self)
{
    return self->lerpStep < 1.0f;
}

void CardStartDrag(struct Card *const self)
{
    if (CardTransitioning(self)) {
        // the card is transitioning from lerpSrc to lerpDst
        // so if the drag is cancelled, go back to lerpDst, where it was originally headed
        self->dragStartPos = self->lerpDst;
    } else {
        self->dragStartPos = self->pos;
    }
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
        self->pos.x = UtilSmoothstep(self->lerpSrc.x, self->lerpDst.x, self->lerpStep);
        self->pos.y = UtilSmoothstep(self->lerpSrc.y, self->lerpDst.y, self->lerpStep);
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
        // || CardTransitioning(self) 
        rectCard.x += 2.0f;
        rectCard.y += 2.0f;
        DrawRectangleRounded(rectCard, 0.05, 9, (Color){0,0,0,63});
        rectCard.x -= 4.0f;
        rectCard.y -= 4.0f;
    }/* else if ( CheckCollisionPointRec(GetMousePosition(), rectCard) ) {
        rectCard.x += 2.0f;
        rectCard.y += 2.0f;
    }*/

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
