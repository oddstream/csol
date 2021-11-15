/* pile.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>

#include "array.h"
#include "baize.h"
#include "scrunch.h"
#include "pile.h"


#define PILE_MAGIC (0xdeadbeef)

// struct Pile* PileNew(const char* category, Vector2 pos, enum FanType fan) {
//     struct Pile* self = calloc(1, sizeof(struct Pile));
//     if ( self ) {
//         self->magic = PILE_MAGIC;
//         strncpy(self->category, category, sizeof self->category - 1);
//         self->pos = PileSetBaizePos(self, pos);
//         self->fan = fan;
//         self->cards = ArrayNew(52);
//     }
//     return self;
// }

static float fanFactors[7] = {
    1.0f,                   // FAN_NONE
    CARD_FACE_FAN_FACTOR_V, // FAN_DOWN
    CARD_FACE_FAN_FACTOR_H, // FAN_LEFT,
    CARD_FACE_FAN_FACTOR_H, // FAN_RIGHT,
    CARD_FACE_FAN_FACTOR_V, // FAN_DOWN3,
    CARD_FACE_FAN_FACTOR_H, // FAN_LEFT3,
    CARD_FACE_FAN_FACTOR_H, // FAN_RIGHT3,
};

void PileCtor(struct Baize *const baize, struct Pile *const self, const char* category, Vector2 slot, enum FanType fan)
{
    self->magic = PILE_MAGIC;
    self->owner = baize;
    strncpy(self->category, category, sizeof self->category - 1);
    self->slot = slot;
    self->fanType = fan;
    self->scrunchDims = (Vector2){0}; // by default, do not scrunch
    self->defaultFanFactor = self->fanFactor = fanFactors[fan];
    self->cards = ArrayNew(52);
}

_Bool PileValid(struct Pile *const self)
{
    return self && self->magic == PILE_MAGIC;
}

struct Baize* PileOwner(struct Pile *const self)
{
    return self->owner;
}

_Bool PileHidden(struct Pile *const self)
{
    return self->slot.x < 0.0f || self->slot.y < 0.0f;
}

_Bool PileEmpty(struct Pile *const self)
{
    return ArrayLen(self->cards) == 0;
}

size_t PileLen(struct Pile *const self)
{
    return ArrayLen(self->cards);
}

void PilePushCard(struct Pile *const self, struct Card* c)
{
    if (PileIsStock(self)) {
        CardFlipDown(c);
    }
    /*
        Can't scrunch here,
        because that would mean repushing the pile's cards,
        which would recurse into here

        TODO not anymore, since Refan
    */
    CardSetOwner(c, self);
    Vector2 fannedPos = PilePosAfter(self, ArrayPeek(self->cards)); // get this *before* pushing card to pile
    CardTransitionTo(c, fannedPos);
    self->cards = ArrayPush(self->cards, c);

    ScrunchPile(self);
}

struct Card* PilePopCard(struct Pile *const self)
{
    struct Card* c = ArrayPop(self->cards);
    if (c) {
        CardSetOwner(c, NULL);
        CardFlipUp(c);
        ScrunchPile(self);
    }
    return c;
}

struct Card* PilePeekCard(struct Pile *const self)
{
    return ArrayPeek(self->cards);
}

struct CardAndIndex PileFindCard(struct Pile *const self, enum CardOrdinal ord, enum CardSuit suit)
{
    size_t index;
    for ( struct Card *c = ArrayFirst(self->cards, &index); c; c = ArrayNext(self->cards, &index) ) {
        if ( c->id.ordinal == ord && c->id.suit == suit ) {
           return (struct CardAndIndex){.card=c, .index=index}; 
        }
    }
    return (struct CardAndIndex){.card=NULL, .index=0};
}

#if 0
struct Card* PileFindLastCardUnderPoint(struct Pile *const self, Vector2 pt)
{
    struct Card *last = NULL;
    size_t index;
    for ( struct Card *c = ArrayFirst(self->cards, &index); c; c = ArrayNext(self->cards, &index) ) {
        if (CheckCollisionPointRec(pt, CardScreenRect(c))) {
            last = c;
        }
    }
    return last;
}
#endif

_Bool PileIsStock(struct Pile *const self)
{
    struct Baize* baize = PileOwner(self);
    if ( !BaizeValid(baize) )
    {
        fprintf(stderr, "ERROR: %s: Pile Baize pointer is not valid\n", __func__);
        return 0;
    }
    if ( !PileValid(baize->stock) )
    {
        fprintf(stderr, "ERROR: %s: Pile Baize stock pointer is not valid\n", __func__);
        return 0;
    }
    return self == baize->stock;
}

// set the position of this pile in baize coords, also sets the auxillary waste pile fanned positions
void PileSetBaizePos(struct Pile *const self, Vector2 pos)
{
    struct Baize *baize = PileOwner(self);
    struct Pack *pack = baize->pack;

    self->pos = pos;
    switch (self->fanType) {
    case FAN_NONE:
    case FAN_DOWN:
    case FAN_LEFT:
    case FAN_RIGHT:
        break;
    case FAN_DOWN3:
        self->pos1 = UtilVector2Add(self->pos, (Vector2){.x = 0.0f, .y = pack->height / CARD_FACE_FAN_FACTOR_V});
        self->pos2 = UtilVector2Add(self->pos1, (Vector2){.x = 0.0f, .y = pack->height / CARD_FACE_FAN_FACTOR_V});
        break;
    case FAN_LEFT3:
        self->pos1 = UtilVector2Add(self->pos, (Vector2){.x = -(pack->width / CARD_FACE_FAN_FACTOR_H), .y = 0.0f});
        self->pos2 = UtilVector2Add(self->pos1, (Vector2){.x = -(pack->width / CARD_FACE_FAN_FACTOR_H), .y = 0.0f});
        break;
    case FAN_RIGHT3:
        self->pos1 = UtilVector2Add(self->pos, (Vector2){.x = pack->width / CARD_FACE_FAN_FACTOR_H, .y = 0.0f});
        self->pos2 = UtilVector2Add(self->pos1, (Vector2){.x = pack->width / CARD_FACE_FAN_FACTOR_H, .y = 0.0f});
        break;
    default:
        break;
    }
}

Vector2 PileBaizePos(struct Pile *const self)
{
    return self->pos;
}

Vector2 PileScreenPos(struct Pile *const self)
{
    return (Vector2){.x = self->pos.x + self->owner->dragOffset.x, .y = self->pos.y + self->owner->dragOffset.y};
}

Rectangle PileBaizeRect(struct Pile *const self)
{
    struct Baize *baize = PileOwner(self);
    struct Pack *pack = baize->pack;
    return (Rectangle){.x = self->pos.x, .y = self->pos.y, .width = pack->width, .height = pack->height};
}

Rectangle PileScreenRect(struct Pile *const self)
{
    struct Baize *baize = PileOwner(self);
    struct Pack *pack = baize->pack;
    return (Rectangle){.x = self->pos.x + self->owner->dragOffset.x, .y = self->pos.y + self->owner->dragOffset.y, .width = pack->width, .height = pack->height};
}

void PileDrawCenteredGlyph(struct Pile *const self, int glyph)
{
    extern Color baizeHighlightColor;

    struct Baize *baize = PileOwner(self);
    struct Pack *pack = baize->pack;

    // TODO consider caching this if too expensive
    GlyphInfo gi = GetGlyphInfo(pack->fontSymbol, glyph);
    Rectangle gr = GetGlyphAtlasRec(pack->fontSymbol, glyph);
    Rectangle rp = PileScreenRect(self);
    rp.x -= gi.offsetX;
    rp.y -= gi.offsetY;
    Vector2 cpos = UtilCenterTextInRectangle(rp, gr.width, gr.height);

    // this may be too expensive to do at 60Hz
    // if ( CheckCollisionPointRec(GetMousePosition(), rp) ) {
    //     cpos.x += 2.0f;
    //     cpos.y += 2.0f;
    // }

    DrawTextCodepoint(pack->fontSymbol, glyph, cpos, pack->pileFontSize, baizeHighlightColor);
}

#if 0
void PileDrawUpperLeftGlyph(struct Pile *const self, int glyph)
{
    extern Font fontSymbol;
    extern int pileFontSize;
    extern Color baizeHighlightColor;

    GlyphInfo gi = GetGlyphInfo(fontSymbol, glyph);
    Rectangle gr = GetGlyphAtlasRec(fontSymbol, glyph);
    Rectangle rp = PileScreenRect(self);
    rp.x -= gi.offsetX;
    rp.y -= gi.offsetY;
    rp.width /= 2.0f;
    rp.height /= 2.0f;
    Vector2 cpos = UtilCenterTextInRectangle(rp, gr.width, gr.height);
    DrawTextCodepoint(fontSymbol, glyph, cpos, pileFontSize, baizeHighlightColor);
}
#endif

#if 0
void PileDrawUpperLeftText(struct Pile *const self, const char *text)
{
    extern Font fontAcme;
    extern int pileFontSize;
    extern Color baizeHighlightColor;

    if (text==NULL || *text=='\0') {
        return;
    }
    Rectangle rp = PileScreenRect(self);
    rp.width /= 2.0f;
    rp.height /= 2.0f;
    Vector2 pos = UtilCenterTextInRectangle(rp, self->labelmte.x, self->labelmte.y);

    DrawTextEx(fontAcme, text, pos, pileFontSize, 1.2f, baizeHighlightColor);
}
#endif

void PileDrawCenteredText(struct Pile *const self, const char *text)
{
    extern float fontSpacing;
    extern Color baizeHighlightColor;

    if (text==NULL || *text=='\0') {
        return;
    }
    struct Baize *baize = PileOwner(self);
    struct Pack *pack = baize->pack;

    Vector2 labelmte = MeasureTextEx(pack->fontAcme, self->label, (float)pack->pileFontSize, fontSpacing);
    Rectangle rp = PileScreenRect(self);
    Vector2 cpos = UtilCenterTextInRectangle(rp, labelmte.x, labelmte.y);

    // this may be too expensive to do at 60Hz
    // if ( CheckCollisionPointRec(GetMousePosition(), rp) ) {
    //     cpos.x += 2.0f;
    //     cpos.y += 2.0f;
    // }

    DrawTextEx(pack->fontAcme, text, cpos, pack->pileFontSize, fontSpacing, baizeHighlightColor);
}

/*
    return the rectangle (in baize coords) that encloses all this pile's cards.
    Used to draw the Pile rounded rect, and to detect card drops on this pile
*/
Rectangle PileFannedBaizeRect(struct Pile *const self)
{
    struct Baize *baize = PileOwner(self);
    struct Pack *pack = baize->pack;

    // cannot use position of top card, in case it's being dragged
    Rectangle r = PileBaizeRect(self);
    if ( ArrayLen(self->cards) > 1 ) {
        struct Card* c = ArrayPeek(self->cards);
        if ( CardDragging(c) ) {
            return r;   // this and the rest are meaningless
        }
        Vector2 cPos = CardBaizePos(c);
        /* K&R style switch formatting, see P59 if you don't believe me */
        switch (self->fanType) {
        case FAN_NONE:
            // do nothing
            break;
        case FAN_RIGHT:
        case FAN_RIGHT3:
            r.width = cPos.x + pack->width - r.x;
            break;
        case FAN_LEFT:
        case FAN_LEFT3:
            r.width = cPos.x - pack->width - r.x;
            break;
        case FAN_DOWN:
        case FAN_DOWN3:
            r.height = cPos.y + pack->height - r.y;
            break;
        default:
            break;
        }
    }
    return r;
}

Rectangle PileFannedScreenRect(struct Pile *const self) {
    Rectangle r = PileFannedBaizeRect(self);
    r.x += self->owner->dragOffset.x;
    r.y += self->owner->dragOffset.y;
    return r;
}

// get the Baize position of the next card to be pushed to this pile, also refans waste pile
Vector2 PilePosAfter(struct Pile *const self, struct Card *const c)
{
    size_t len = ArrayLen(self->cards);
    if (len == 0) {
        return self->pos;
    }

    struct Baize *baize = PileOwner(self);
    struct Pack *pack = baize->pack;
    Vector2 pos;
    size_t i;

    /* K&R style switch formatting, see P59 if you don't believe me */
    switch (self->fanType) {
    case FAN_NONE:
        pos = self->pos;
        break;
    case FAN_DOWN:
        pos = CardTransitioning(c) ? c->lerpDst : c->pos;
        pos.y += (c->prone) ? pack->height / CARD_BACK_FAN_FACTOR : pack->height / self->fanFactor;
        break;
    case FAN_LEFT:
        pos = CardTransitioning(c) ? c->lerpDst : c->pos;
        pos.x -= (c->prone) ? pack->width / CARD_BACK_FAN_FACTOR : pack->width / self->fanFactor;
        break;
    case FAN_RIGHT:
        pos = CardTransitioning(c) ? c->lerpDst : c->pos;
        pos.x += (c->prone) ? pack->width / CARD_BACK_FAN_FACTOR : pack->width / self->fanFactor;
        break;
    case FAN_DOWN3:
    case FAN_LEFT3:
    case FAN_RIGHT3:
        switch (ArrayLen(self->cards)) {
        case 0:
            pos = self->pos; // won't happen, empty pile detected on entry
            break;
        case 1:
            pos = self->pos1;// incoming card at slot[1]
            break;
        case 2:
            pos = self->pos2; // incoming card at slot [2]
            break;
        default:
            pos = self->pos2; // incoming card at slot [2]
            // top card needs to transition from slot[2] to slot[1]
            i = ArrayLen(self->cards) - 1;
            CardTransitionTo(ArrayGet(self->cards, i), self->pos1);
            // mid card needs to transition from slot[1] to slot[0]
            // all other cards to slot[0]
            do {
                CardTransitionTo(ArrayGet(self->cards, --i), self->pos);
            } while (i);
            break;
        }
    default:
        // we covered all cases, there is no default
        // this is just to keep the complier quiet
        break;
    }
    return pos;
}

void PileRefan(struct Pile *const self)
{
    _Bool doFan3 = 0;
    size_t index;
    struct Card *c = ArrayFirst(self->cards, &index);
    if (!c) {
        return;
    }
    CardTransitionTo(c, self->pos);
    switch (self->fanType) {
    case FAN_NONE:
        while ((c = ArrayNext(self->cards, &index))) {
            CardTransitionTo(c, self->pos);
        }
        break;
    case FAN_DOWN3:
    case FAN_LEFT3:
    case FAN_RIGHT3:
        while ((c = ArrayNext(self->cards, &index))) {
            CardTransitionTo(c, self->pos);
        }
        doFan3 = 1;
        break;
    case FAN_DOWN:
    case FAN_LEFT:
    case FAN_RIGHT:
        while ((c = ArrayNext(self->cards, &index))) {
            CardTransitionTo(c, PilePosAfter(self, ArrayGet(self->cards, index-1)));
        }
        break;
    default:
        break;
    }

    if (doFan3) {
        switch (ArrayLen(self->cards)) {
        case 0:
            // won't happen; nothing to do
            break;
        case 1:
            // nothing to do
            break;
        case 2:
            index = ArrayLen(self->cards);
            c = ArrayGet(self->cards, --index);
            CardTransitionTo(c, self->pos1);
            break;
        default:
            // three or more
            index = ArrayLen(self->cards);
            c = ArrayGet(self->cards, --index);
            CardTransitionTo(c, self->pos2);
            c = ArrayGet(self->cards, --index);
            CardTransitionTo(c, self->pos1);
            break;
        }
    }
}

_Bool PileMoveCard(struct Pile *const self, struct Pile *const src)
{
    // an optimized, single card version of PileMoveCards
    struct Card *const c = PilePopCard(src);
    if (!c) {
        // fprintf(stderr, "WARNING: %s: source pile is empty\n", __func__);
        return 0;
    }

    PilePushCard(self, c);

    // flip up an exposed source card, if src pile is not Stock
    if (!PileIsStock(src)) {
        struct Card* tc = PilePeekCard(src);
        if ( tc ) {
            CardFlipUp(tc);
        }
    }

    // special case: waste may need refanning if we took a card from it
    if ( src->fanType == FAN_DOWN3 || src->fanType == FAN_LEFT3 || src->fanType == FAN_RIGHT3 ) {
        PileRefan(src);
    }

    ScrunchPile(self);

    return 1;
}

_Bool PileMoveCards(struct Pile *const self, struct Card const* c)
{
    // move cards to this pile

    struct Pile *src = c->owner;    // TODO const errors when using CardOwner()
    if (src==self) {
        fprintf(stderr, "ERROR: %s: src and dst are the same\n", __func__);
        return 0;
    }
    size_t oldSrcLen = PileLen(src), newSrcLen;
    if (!ArrayIndexOf(src->cards, c, &newSrcLen)) {
        fprintf(stderr, "ERROR: %s: could not find card in pile\n", __func__);
        return 0;
    }

    // pop the tail off the source and push onto tmp stack
    struct Array* tmp = ArrayNew(PileLen(self) + PileLen(src));
    while ( PileLen(src) != newSrcLen ) {
        tmp = ArrayPush(tmp, PilePopCard(src));
    }

    // TODO make some noise

    // pop all cards off the tmp stack and onto the destination (self)
    while ( ArrayLen(tmp) ) {
        PilePushCard(self, ArrayPop(tmp));
    }
    ArrayFree(tmp);

    // if ( newSrcLen != oldSrcLen ) {
    //     fprintf(stderr, "Something went wrong moving cards from %s to %s\n", src->category, self->category);
    // }
    // fprintf(stderr, "old %lu, new %lu\n", oldSrcLen, newSrcLen);

    // flip up an exposed source card, if src pile is not Stock
    if (!PileIsStock(src)) {
        struct Card* tc = PilePeekCard(src);
        if (tc) {
            CardFlipUp(tc);
        }
    }

    // special case: waste may need refanning if we took a card from it
    if ( src->fanType == FAN_DOWN3 || src->fanType == FAN_LEFT3 || src->fanType == FAN_RIGHT3 ) {
        PileRefan(src);
    }

    ScrunchPile(self);

    if (newSrcLen == oldSrcLen) {
        fprintf(stderr, "ERROR: %s: did nothing?\n", __func__);
        return 0;
    }

    return newSrcLen != oldSrcLen;
}

#if 0
void PileRepushAllCards(struct Pile *const self)
{
    if (PileEmpty(self)) {
        return;
    }
    struct Array *tmp = ArrayClone(self->cards);
    if (tmp) {
        ArrayReset(self->cards);

        size_t index;
        for ( struct Card *c = ArrayFirst(tmp, &index); c; c = ArrayNext(tmp, &index) ) {
            PilePushCard(self, c);
        }
        ArrayFree(tmp);
    }
}
#endif

