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
//         self->pos = pos;
//         self->fan = fan;
//         self->cards = ArrayNew(52);
//     }
//     return self;
// }

void PileCtor(struct Baize *const baize, struct Pile *const self, const char* category, Vector2 slot, enum FanType fan)
{
    self->magic = PILE_MAGIC;
    self->owner = baize;
    strncpy(self->category, category, sizeof self->category - 1);
    self->slot = slot;
    self->fanType = fan;
    self->scrunchLimit = -1.0f; // by default, do not scrunch
    PileResetFanFactor(self);
    self->defaultFanFactor = self->fanFactor;
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

void PileResetFanFactor(struct Pile *const self)
{
    switch (self->fanType) {
        case FAN_NONE:
            self->fanFactor = 1.0f;
            break;
        case FAN_DOWN:
            self->fanFactor = CARD_FACE_FAN_FACTOR_V;
            break;
        case FAN_LEFT:
            self->fanFactor = CARD_FACE_FAN_FACTOR_H;
            break;
        case FAN_RIGHT:
            self->fanFactor = CARD_FACE_FAN_FACTOR_H;
            break;
        case FAN_DOWN3:
            self->fanFactor = CARD_FACE_FAN_FACTOR_V;
            break;
        case FAN_LEFT3:
            self->fanFactor = CARD_FACE_FAN_FACTOR_H;
            break;
        case FAN_RIGHT3:
            self->fanFactor = CARD_FACE_FAN_FACTOR_H;
            break;
        default:
            break;
    }
}

void PilePushCard(struct Pile *const self, struct Card* c)
{
    if ( PileIsStock(self) ) {
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

    if (ScrunchOverflow(self, fannedPos)) {
        self->fanFactor += 1.0f;
        if (self->fanFactor > MAX_FAN_FACTOR) {
            self->fanFactor = MAX_FAN_FACTOR;
        }
        PileRefan(self);
    }
}

struct Card* PilePopCard(struct Pile *const self)
{
    struct Card* c = ArrayPop(self->cards);
    if (c) {
        if (self->fanFactor != self->defaultFanFactor) {
            self->fanFactor = self->defaultFanFactor;
            PileRefan(self);
        }
        CardSetOwner(c, NULL);
        CardFlipUp(c);
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
    extern float cardWidth, cardHeight;
    return (Rectangle){.x = self->pos.x, .y = self->pos.y, .width = cardWidth, .height = cardHeight};
}

Rectangle PileScreenRect(struct Pile *const self)
{
    extern float cardWidth, cardHeight;
    return (Rectangle){.x = self->pos.x + self->owner->dragOffset.x, .y = self->pos.y + self->owner->dragOffset.y, .width = cardWidth, .height = cardHeight};
}

#if 0
Vector2 PileCalculatePosFromSlot(struct Pile *const self)
{
    extern float leftMargin, topMargin, pilePaddingX, pilePaddingY, cardWidth, cardHeight;

    return (Vector2){
                .x = leftMargin + (self->slot.x * (cardWidth + pilePaddingX)),
                .y = topMargin + (self->slot.y * (cardHeight + pilePaddingY)),
            };
}
#endif

void PileDrawCenteredGlyph(struct Pile *const self, int glyph)
{
    extern Font fontSymbol;
    extern int pileFontSize;
    extern Color baizeHighlightColor;

    // TODO consider caching this if too expensive
    GlyphInfo gi = GetGlyphInfo(fontSymbol, glyph);
    Rectangle gr = GetGlyphAtlasRec(fontSymbol, glyph);
    Rectangle rp = PileScreenRect(self);
    rp.x -= gi.offsetX;
    rp.y -= gi.offsetY;
    Vector2 cpos = UtilCenterTextInRectangle(rp, gr.width, gr.height);

    // this may be too expensive to do at 60Hz
    // if ( CheckCollisionPointRec(GetMousePosition(), rp) ) {
    //     cpos.x += 2.0f;
    //     cpos.y += 2.0f;
    // }

    DrawTextCodepoint(fontSymbol, glyph, cpos, pileFontSize, baizeHighlightColor);
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
    extern Font fontAcmePile;
    extern int pileFontSize;
    extern float fontSpacing;
    extern Color baizeHighlightColor;

    if (text==NULL || *text=='\0') {
        return;
    }
    // TODO consider caching this if too expensive
    Rectangle rp = PileScreenRect(self);
    Vector2 cpos = UtilCenterTextInRectangle(rp, self->labelmte.x, self->labelmte.y);

    // this may be too expensive to do at 60Hz
    // if ( CheckCollisionPointRec(GetMousePosition(), rp) ) {
    //     cpos.x += 2.0f;
    //     cpos.y += 2.0f;
    // }

    DrawTextEx(fontAcmePile, text, cpos, pileFontSize, fontSpacing, baizeHighlightColor);
}

/*
    Used to draw the Pile rounded rect, and to detect card drops on this pile
*/
Rectangle PileFannedBaizeRect(struct Pile *const self)
{
    // cannot use position of top card, in case it's being dragged
    extern float cardWidth, cardHeight;
    Rectangle r = PileBaizeRect(self);
    if ( ArrayLen(self->cards) > 1 ) {
        struct Card* c = ArrayPeek(self->cards);
        if ( CardDragging(c) ) {
            return r;   // this and the rest are meaningless
        }
        Vector2 cPos = CardBaizePos(c);
        switch ( self->fanType ) {
            case FAN_NONE:
                // do nothing
                break;
            case FAN_RIGHT:
            case FAN_RIGHT3:
                r.width = cPos.x + cardWidth - r.x;
                break;
            case FAN_LEFT:
            case FAN_LEFT3:
                r.width = cPos.x - cardWidth - r.x;
                break;
            case FAN_DOWN:
            case FAN_DOWN3:
                r.height = cPos.y + cardHeight - r.y;
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
    extern float cardWidth, cardHeight;

    size_t len = ArrayLen(self->cards);
    if (len == 0) {
        return self->pos;
    }

    // struct Card *c;
    Vector2 pos, slot0, slot1, slot2;
    size_t i;

    switch (self->fanType) {
        case FAN_NONE:
            pos = self->pos;
            break;
        case FAN_DOWN:
            // c = ArrayPeek(self->cards);
            pos = CardTransitioning(c) ? c->lerpDst : c->pos;
            pos.y += (c->prone) ? cardHeight / CARD_BACK_FAN_FACTOR : cardHeight / self->fanFactor;
            break;
        case FAN_LEFT:
            // c = ArrayPeek(self->cards);
            pos = CardTransitioning(c) ? c->lerpDst : c->pos;
            pos.x -= (c->prone) ? cardWidth / CARD_BACK_FAN_FACTOR : cardWidth / self->fanFactor;
            break;
        case FAN_RIGHT:
            // c = ArrayPeek(self->cards);
            pos = CardTransitioning(c) ? c->lerpDst : c->pos;
            pos.x += (c->prone) ? cardWidth / CARD_BACK_FAN_FACTOR : cardWidth / self->fanFactor;
            break;
        case FAN_DOWN3:
            slot0 = self->pos;
            slot1 = UtilVector2Add(slot0, (Vector2){.x = cardWidth / CARD_FACE_FAN_FACTOR_V, .y = cardHeight / self->fanFactor});
            slot2 = UtilVector2Add(slot1, (Vector2){.x = cardWidth / CARD_FACE_FAN_FACTOR_V, .y = cardHeight / self->fanFactor});

            pos.x = slot0.x;
            switch (ArrayLen(self->cards)) {
                case 0:
                    pos.y = slot0.y; // won't happen, empty pile detected on entry
                    break;
                case 1:
                    pos.y = slot1.y; // incoming card at slot[1]
                    break;
                case 2:
                    pos.y = slot2.y; // incoming card at slot [2]
                    break;
                default:
                    pos.y = slot2.y; // incoming card at slot [2]
                    // top card needs to transition from slot[2] to slot[1]
                    i = ArrayLen(self->cards) - 1;
                    CardTransitionTo(ArrayGet(self->cards, i), (Vector2){.x=slot0.x, .y=slot1.y});
                    // mid card needs to transition from slot[1] to slot[0]
                    do {
                        CardTransitionTo(ArrayGet(self->cards, --i), self->pos);
                    } while (i);
                    break;
            }
            break;
        case FAN_LEFT3:
            pos = self->pos;    // TODO not implemented yet
            break;
        case FAN_RIGHT3:
            slot0 = self->pos;
            slot1 = UtilVector2Add(slot0, (Vector2){.x = cardWidth / self->fanFactor, .y = cardHeight / CARD_FACE_FAN_FACTOR_V});
            slot2 = UtilVector2Add(slot1, (Vector2){.x = cardWidth / self->fanFactor, .y = cardHeight / CARD_FACE_FAN_FACTOR_V});

            pos.y = slot0.y;
            switch (ArrayLen(self->cards)) {
                case 0:
                    pos.x = slot0.x;
                    break;
                case 1:
                    pos.x = slot1.x;
                    break;
                case 2:
                    pos.x = slot2.x;
                    break;
                default:
                    pos.x = slot2.x;
                    i = ArrayLen(self->cards) - 1;
                    CardTransitionTo(ArrayGet(self->cards, i), (Vector2){.x=slot1.x, .y=slot0.y});
                    do {
                        CardTransitionTo(ArrayGet(self->cards, --i), self->pos);
                    } while (i);
                    break;
            }
            break;
        default:
            break;
    }
    return pos;
}

void PileRefan(struct Pile *const self)
{
    size_t index;
    struct Card *c = ArrayFirst(self->cards, &index);
    if (c) {
        CardTransitionTo(c, self->pos);
        while ((c = ArrayNext(self->cards, &index))) {
            CardTransitionTo(c, PilePosAfter(self, ArrayGet(self->cards, index-1)));
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
    if ( !PileIsStock(src) ) {
        struct Card* tc = PilePeekCard(src);
        if ( tc ) {
            CardFlipUp(tc);
        }
    }

    // special case: waste may need refanning if we took a card from it
    if ( src->fanType == FAN_DOWN3 || src->fanType == FAN_LEFT3 || src->fanType == FAN_RIGHT3 ) {
        PileRefan(src);
    }

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

    // TODO scrunch

    if (newSrcLen == oldSrcLen) {
        fprintf(stderr, "WARNING: %s did nothing?\n", __func__);
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

_Bool PileInertCanMoveTail(struct Array *const tail)
{
    (void)tail;
    return 0;
}

_Bool PileInertCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    (void)baize;
    (void)self;
    (void)c;
    return 0;
}

_Bool PileInertCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    (void)baize;
    (void)self;
    (void)tail;
    return 0;
}

void PileInertPileTapped(struct Pile *const self)
{
    (void)self;
}

void PileInertTailTapped(struct Pile *const self, struct Array *const tail)
{
    (void)self;
    (void)tail;
}

void PileGenericTailTapped(struct Pile *const self, struct Array *const tail)
{
    struct Baize* baize = PileOwner(self);
    size_t index;
    for ( struct Pile* fp = ArrayFirst(baize->foundations, &index); fp; fp = ArrayNext(baize->foundations, &index) ) {
        if ( fp->vtable->CanAcceptTail(baize, fp, tail) ) {
            struct Card *c = ArrayGet(tail, 0);
            PileMoveCard(fp, CardOwner(c));
            break;
        }
    }
}

int PileInertCollect(struct Pile *const self)
{
    (void)self;
    return 0;
}

int PileGenericCollect(struct Pile *const self)
{
    // Collect is like tapping on the top card of each pile (except Stock), or on a K in a Spider pile
    // prefer to collect a run of cards from one pile to one foundation

    // NB Spider piles are not collected because moving them to the 'foundations' is optional according to Morehead and Mott-Smith
    // so Spider games have Discard piles, not Foundation piles
    // Spider could be complete when a Tableau is either empty or contains 13 conformant cards (TODO not currently implemented)
    struct Baize* baize = PileOwner(self);
    int cardsMoved = 0;
    size_t index;
    for ( struct Pile* fp = ArrayFirst(baize->foundations, &index); fp; fp = ArrayNext(baize->foundations, &index) ) {
        for (;;) {
            struct Card *c = PilePeekCard(self);
            if (!c) {
                // this pile is empty
                return cardsMoved;
            }
            if ( !fp->vtable->CanAcceptCard(baize, fp, c) ) {
                // this Foundation doesn't want this card; onto the next Foundation
                break;
            }
            if ( PileMoveCard(fp, self) ) {
                cardsMoved++;
            }
        }
    }
    return cardsMoved;
}

_Bool PileInertComplete(struct Pile *const self)
{
    (void)self;
    return 1;
}

_Bool PileInertConformant(struct Pile *const self)
{
    (void)self;
    return 1;
}

void PileInertSetLabel(struct Pile *const self, const char *label)
{
    (void)self;
    (void)label;
}

void PileGenericSetLabel(struct Pile *const self, const char *label)
{
    extern Font fontAcmePile;
    extern int pileFontSize;
    extern float fontSpacing;

    // fprintf(stdout, "INFO: %s: set %s label to '%s'\n", __func__, self->category, label);

    if (pileFontSize==0) fprintf(stderr, "ERROR: %s: pileFontSize is zero\n", __func__);
    memset(self->label, 0, MAX_PILE_LABEL + 1);
    if (*label) {
        strncpy(self->label, label, MAX_PILE_LABEL);
        self->labelmte = MeasureTextEx(fontAcmePile, self->label, (float)pileFontSize, fontSpacing);
    } else {
        self->labelmte = (Vector2){0};
    }
}

void PileInertSetRecycles(struct Pile *const self, int r)
{
    (void)self;
    (void)r;
}

void PileInertCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
    (void)self;
    (void)sorted;
    (void)unsorted;
}

void PileUpdate(struct Pile *const self)
{
    ArrayForeach(self->cards, (ArrayIterFunc)CardUpdate);
}

void PileDraw(struct Pile *const self)
{
    extern Color baizeHighlightColor;

    // BeginDrawing() has been called by BaizeDraw()
    // Rectangle r = PileFannedScreenRect(self);
    Rectangle r = PileScreenRect(self);
    DrawRectangleRoundedLines(r, 0.05, 9, 2.0, baizeHighlightColor);

    PileDrawCenteredText(self, self->label);
}

void PileFree(struct Pile *const self)
{
    // Card objects exist in the Baize->cardLibrary array, so we don't free them here
    if ( self ) {
        ArrayFree(self->cards);
        self->magic = 0;
        free(self);
    }
}
