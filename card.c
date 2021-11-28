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
#define FLIPSTEPAMOUNT (0.075f)
// #define FLIPSTEPAMOUNT (0.0075f)
#define DEBUG_SPEED  (0.005f)
#define SLOW_SPEED (0.01f)
#define NORMAL_SPEED (0.02f)
#define FAST_SPEED (0.04f)

// https://www.fileformat.info/info/unicode/block/playing_cards/fontsupport.htm
// annoyingly, the unicode playing cards include 'Knights' (C) between Jacks and Queens
// cards are in the same order as used for spritesheets, so we can use card->frame
int cardCodePoints[52] = {
    // Clubs
    0x1F0D1, 0x1F0D2, 0x1F0D3, 0x1F0D4, 0x1F0D5, 0x1F0D6, 0x1F0D7, 0x1F0D8, 0x1F0D9, 0x1F0DA, 0x1F0DB, /* 0x1F0DC, */ 0x1F0DD, 0x1F0DE,
    // Diamonds
    0x1F0C1, 0x1F0C2, 0x1F0C3, 0x1F0C4, 0x1F0C5, 0x1F0C6, 0x1F0C7, 0x1F0C8, 0x1F0C9, 0x1F0CA, 0x1F0CB, /* 0x1F0CC, */ 0x1F0CD, 0x1F0CE,
    // Hearts
    0x1F0B1, 0x1F0B2, 0x1F0B3, 0x1F0B4, 0x1F0B5, 0x1F0B6, 0x1F0B7, 0x1F0B8, 0x1F0B9, 0x1F0BA, 0x1F0BB, /* 0x1F0BC, */ 0x1F0BD, 0x1F0BE,
    // Spades
    0x1F0A1, 0x1F0A2, 0x1F0A3, 0x1F0A4, 0x1F0A5, 0x1F0A6, 0x1F0A7, 0x1F0A8, 0x1F0A9, 0x1F0AA, 0x1F0AB, /* 0x1F0AC, */ 0x1F0AD, 0x1F0AE,
};

Color fourColors[4] = {
    {0,0,0,0xff},           // black
    {0xB2,0x22,0x22,0xff},  // Firebrick
    {153,50,204,0xff},      // darkorchid
    {0,100,0,0xff},         // darkgreen
};

Color twoColors[4] = {
    {0,0,0,0xff},           // black
    {0xB2,0x22,0x22,0xff},  // Firebrick
    {0xB2,0x22,0x22,0xff},  // Firebrick
    {0,0,0,0xff},           // black
};

struct Card CardNew(unsigned pack, enum CardOrdinal ord, enum CardSuit suit)
{
    // create cards face down, because that's how they'll be in the Stock
    struct Card self = {.magic = CARD_MAGIC, .owner = NULL, .id.pack = pack, .id.ordinal = ord, .id.suit = suit, .prone = 1, .dragging = 0};
    self.frame = (suit * 13) + (ord - 1); // index into Vector2D info struct, or retro spritesheet
    self.pos = (Vector2){0};
    self.flipWidth = 1.0f;
    self.flipStep = 0.0f;
    self.dragStartPos = self.pos;
    self.lerpSrc = self.pos;
    self.lerpDst = self.pos;
    self.lerpStep = 1.0f;   // not currently lerping
    return self;    // return whole struct by value, gets saved into cardLibrary[]
}

_Bool CardValid(struct Card *const self)
{
    return self && self->magic == CARD_MAGIC;
}

void CardSetOwner(struct Card *const self, struct Pile *const pile)
{
    self->owner = pile;
}

struct Pile* CardOwner(struct Card *const self)
{
    return self->owner;
}

enum CardOrdinal CardOrdinal(struct Card *const self)
{
    return self->id.ordinal;
}

enum CardSuit CardSuit(struct Card *const self)
{
    return self->id.suit;
}

_Bool CardProne(struct Card *const self)
{
    return self->prone;
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
    struct Baize* baize = PileOwner(CardOwner(self));
    return (Vector2){.x = self->pos.x + baize->dragOffset.x, .y = self->pos.y + baize->dragOffset.y};
}

Rectangle CardBaizeRect(struct Card *const self)
{
    struct Baize* baize = PileOwner(CardOwner(self));
    return (Rectangle){.x = self->pos.x, .y = self->pos.y, .width = baize->pack->width, .height = baize->pack->height};
}

Rectangle CardScreenRect(struct Card *const self)
{
    struct Baize* baize = PileOwner(CardOwner(self));
    Vector2 csp = CardScreenPos(self);
    return (Rectangle){.x=csp.x, .y=csp.y, .width=baize->pack->width, .height=baize->pack->height}; 
}

void CardMovePositionBy(struct Card *const self, Vector2 delta)
{
    // fprintf(stdout, "CardDragBy %.0f, %.0f\n", delta.x, delta.y);
    self->pos.x += delta.x;
    self->pos.y += delta.y;
}

void CardTransitionTo(struct Card *const self, Vector2 pos)
{
    extern int flag_nolerp;
    if (flag_nolerp) {
        self->pos = pos;
        self->lerpDst = pos;    // paranoia
        self->lerpStep = 1.0f;  // stop any current lerp (again, paranoia)
        return;
    }

    struct Baize* baize = PileOwner(CardOwner(self));

    if ( pos.x == self->pos.x && pos.y == self->pos.y ) {
        self->lerpStep = 1.0f;    // stop any current lerp
        return;
    }
    self->lerpSrc = self->pos;
    self->lerpDst = pos;
#if 1
    if (UtilDistance(self->pos, pos) < baize->pack->width) {
        self->lerpStepAmount = FAST_SPEED;
        self->lerpFunc = &UtilLerp;
    } else {
        self->lerpStepAmount = NORMAL_SPEED;
        self->lerpFunc = &UtilSmoothstep;
    }
#else
    // doing this makes the transition drawing order look wrong and ugly
    const float speed = 5.0f;
    float dist = UtilDistance(self->lerpSrc, self->lerpDst);
    self->lerpStepAmount = fminf(0.025f, speed*(1.0f/dist));
#endif
    self->lerpStep = 0.1f;       // trigger a lerp
}

_Bool CardTransitioning(struct Card *const self)
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
    self->dragging = 1;
}

void CardStopDrag(struct Card *const self)
{
    self->dragging = 0;
}

void CardCancelDrag(struct Card *const self)
{
    self->dragging = 0;
    CardTransitionTo(self, self->dragStartPos);
}

_Bool CardWasDragged(struct Card *const self)
{
    return self->pos.x != self->dragStartPos.x || self->pos.y != self->dragStartPos.y;
}

_Bool CardDragging(struct Card *const self)
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
        {
            self->pos.x = UtilSmoothstep(self->lerpSrc.x, self->lerpDst.x, self->lerpStep);
            self->pos.y = UtilSmoothstep(self->lerpSrc.y, self->lerpDst.y, self->lerpStep);
            self->lerpStep += self->lerpStepAmount;
            if ( self->lerpStep >= 1.0f ) {
                self->pos = self->lerpDst;
            }
        }
    }
}

static void DrawUnicodeCard(struct Card *const self, struct Pack *const pack, _Bool showFace)
{
    int glyph;
    Color cardColor;
    if (showFace) {
        glyph = cardCodePoints[self->frame];
        cardColor = pack->numberOfColors == 4 ? fourColors[self->id.suit] : twoColors[self->id.suit];
    } else {
        glyph = 0x1F0A0;
        cardColor = (Color){70,130,180,0xff};   // SteelBlue
    }
    GlyphInfo gi = GetGlyphInfo(pack->unicodeFont, glyph);
    Rectangle gr = GetGlyphAtlasRec(pack->unicodeFont, glyph);
    Rectangle rc = CardScreenRect(self);
    
    // Rectangle rc2 = rc;
    // rc2.width *= self->flipWidth;
    // rc2.x += (rc.width - rc2.width) / 2.0f;
    DrawRectangleRounded(rc, pack->roundness, 9, (Color){0xf5,0xf5,0xf5,0xff});   // Whitesmoke

    rc.x -= gi.offsetX;
    rc.y -= gi.offsetY;

    Vector2 cpos = UtilCenterTextInRectangle(rc, gr.width, gr.height);
    DrawTextCodepoint(pack->unicodeFont, glyph, cpos, pack->width * pack->unicodeFontExpansion, cardColor);
}

#if 0
static void DrawScaledCard(struct Card *const self, struct Pack *const pack, _Bool showFace)
{
    static int suitSymbol[4] = {
        0x2663, /* black club */
        0x2666, /* black diamond */
        0x2665, /* black heart */
        0x2660, /* black spade */
    };

    Rectangle rc = CardScreenRect(self);
    if (showFace) {
        DrawRectangleRounded(rc, pack->roundness, 9, (Color){245,245,245,255});   // Whitesmoke
        DrawRectangleRoundedLines(rc, pack->roundness, 9, 2.0f, (Color){192,192,192,255}); // Silver
        DrawTextEx(pack->pileFont,
            UtilOrdToShortString(CardOrdinal(self)),
            (Vector2){10.0f + rc.x, rc.y},
            rc.height / 2.0f,
            1.0f,
            twoColors[CardSuit(self)]
        );
        DrawTextCodepoint(pack->symbolFont,
            suitSymbol[CardSuit(self)],
            (Vector2){rc.x + rc.width / 2.0f, rc.y},
            rc.height / 2.0f,
            twoColors[CardSuit(self)]
        );
    } else {
        DrawRectangleRounded(rc, pack->roundness, 9, (Color){100,149,237,255});   // CornflowerBlue
        DrawRectangleRoundedLines(rc, pack->roundness, 9, 2.0f, (Color){192,192,192,255}); // Silver
    }
}
#endif

void CardDraw(struct Card *const self)
{
    // BeginDrawing() has been called by BaizeDraw()
    struct Baize* baize = PileOwner(CardOwner(self));
    struct Pack *pack = baize->pack;

    _Bool showFace;

    // card prone has already been set to destination state
    if ( self->flipStep < 0.0f ) {
        if ( self->prone ) {
            // card is getting narrower, and it's going to show face down, but show face up
            showFace = 1;
        } else {
            // card is getting narrower, and it's going to show face up, but show face down
            showFace = 0;
        }
    } else {
        if ( self->prone ) {
            showFace = 0;
        } else {
            showFace = 1;
        }
    }

    Rectangle r = CardScreenRect(self);

    if ( CardDragging(self) ) {
        // || CardTransitioning(self) 
        r.x += 2.0f;
        r.y += 2.0f;
        DrawRectangleRounded(r, pack->roundness, 9, (Color){0,0,0,63});
        r.x -= 4.0f;
        r.y -= 4.0f;
    }

#if 0
    /*
        experimental feature; make the cards move a little when the mouse cursor is over them
        + indicates that the cards can be moved, provides visual feedback
        - seems a little naff, of course the cards can be moved
    */
    if (!self->owner->owner->tail) {
    // if (!(CardDragging(self) || CardTransitioning(self))) {
        if (PileFindLastCardUnderPoint(self->owner, GetMousePosition()) == self) {
            r.x -= 1.0f;
            r.y -= 1.0f;
        }
    }
#endif

    /*
        tried angling the card if it was dragging or transitioning,
        but it brought out the jaggies and looked a bit gimmicky
    */

    /*
        would like to draw scaled cards into a spritesheet Image,
        and then load that spritesheet into a Texture with LoadTextureFromImage(),
        which is all possible (and has been prototyped)
        BUT
        raylib has no way to draw rounded rectangles into an Image
        (and I can't be arsed to write my own at the moment)
        and raylib has no way to draw a codepoint glyph into an Image
        (so there'll be no scalable suit symbols)
        SO
        drawed scaled cards dynamically, in a low huff.
        can see the frame rate hit when running under valgrind
        (as you can when drawing unicode cards)
        AND
        because they are drawn with text things and no spritesheet
        they can't be flipped
    */

    if ((pack->ssFace != NULL) & (pack->ssBack != NULL)) {
        if (showFace) {
            SpritesheetDraw(pack->ssFace, self->frame, self->flipWidth, 0.0f, r);
        } else {
            SpritesheetDraw(pack->ssBack, pack->backFrame, self->flipWidth, 0.0f, r);
        }
    } else if (pack->unicodeFont.baseSize) {
        DrawUnicodeCard(self, pack, showFace);
    } else {
        // DrawScaledCard(self, pack, showFace);
    }
}

void CardFlipUp(struct Card *const self)
{
    extern int flag_noflip;

    if ( self->prone ) {
        self->prone = 0;
        if (!flag_noflip) {
            self->flipStep = -FLIPSTEPAMOUNT;   // start by making card narrower ...
            self->flipWidth = 1.0f;             // ... from it's full width
        }
    }
}

void CardFlipDown(struct Card *const self)
{
    extern int flag_noflip;

    if ( !self->prone ) {
        self->prone = 1;
        if (!flag_noflip) {
            self->flipStep = -FLIPSTEPAMOUNT;   // start by making card narrower ...
            self->flipWidth = 1.0f;             // ... from it's full width
        }
    }
}

_Bool CardFlipping(struct Card *const self)
{
    return self->flipStep != 0.0f;
}

void CardFree(struct Card *const self)
{
    // struct Card doesn't contain any allocated objects, so not much to do here
    self->magic = 0;
}
