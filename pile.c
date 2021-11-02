/* pile.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <baize.h>
#include "pile.h"
#include "array.h"

#define PILE_MAGIC (0xdeadbeef)

#define CARD_FACE_FAN_FACTOR (3.0f)
#define CARD_BACK_FAN_FACTOR (5.0f)

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
    if ( PileIsStock(self) ) {
        CardFlipDown(c);
    }
    CardSetOwner(c, self);
    Vector2 fannedPos = PilePushedFannedPos(self); // get this *before* pushing card to pile
    // if ( strcmp(self->category, "Waste") == 0 ) {
    //     fprintf(stdout, "Push from %.0f, %.0f to %.0f, %.0f\n", c->pos.x, c->pos.y, fannedPos.x, fannedPos.y);
    // }
    CardTransitionTo(c, fannedPos);
    // CardSetPos(c, fannedPos);
    self->cards = ArrayPush(self->cards, c);
}

struct Card* PilePopCard(struct Pile *const self)
{
    struct Card* c = (struct Card*)ArrayPop(self->cards);
    if ( CardValid(c) ) {
        CardSetOwner(c, NULL);
        CardFlipUp(c);
    }
    return c;
}

struct Card* PilePeekCard(struct Pile *const self)
{
    return (struct Card*)ArrayPeek(self->cards);
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
    struct Baize* baize = self->owner;
    if ( !BaizeValid(baize) )
    {
        fprintf(stderr, "ERROR: %s: Pile Baize pointer is not valid\n", __func__);
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

Vector2 PileCalculatePosFromSlot(struct Pile *const self)
{
    extern float leftMargin, topMargin, pilePaddingX, pilePaddingY, cardWidth, cardHeight;

    return (Vector2){
                .x = leftMargin + (self->slot.x * (cardWidth + pilePaddingX)),
                .y = topMargin + (self->slot.y * (cardHeight + pilePaddingY)),
            };
}

Rectangle PileFannedBaizeRect(struct Pile *const self)
{
    // cannot use position of top card, in case it's being dragged
    extern float cardWidth, cardHeight;
    Rectangle r = PileBaizeRect(self);
    if ( ArrayLen(self->cards) > 2 ) {
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

Vector2 PilePushedFannedPos(struct Pile *const self)
{   // In function ‘PilePushedFannedPos’: error: assuming signed overflow does not occur when changing X +- C1 cmp C2 to X cmp C2 -+ C1 [-Werror=strict-overflow]
    extern float cardWidth, cardHeight;

    Vector2 pos = self->pos;
    float faceDelta, backDelta;
    struct Card* c;
    size_t index = 0;

    switch ( self->fanType ) {
        case FAN_NONE:
            // do nothing
            break;
        case FAN_RIGHT:
            faceDelta = cardWidth / CARD_FACE_FAN_FACTOR;
            backDelta = cardWidth / CARD_BACK_FAN_FACTOR;
            c = (struct Card*)ArrayFirst(self->cards, &index);
            while ( c ) {
                pos.x += c->prone ? backDelta : faceDelta;
                c = (struct Card*)ArrayNext(self->cards, &index);
            }
            break;
        case FAN_LEFT:
            faceDelta = cardWidth / CARD_FACE_FAN_FACTOR;
            backDelta = cardWidth / CARD_BACK_FAN_FACTOR;
            c = (struct Card*)ArrayFirst(self->cards, &index);
            while ( c ) {
                pos.x -= c->prone ? backDelta : faceDelta;
                c = (struct Card*)ArrayNext(self->cards, &index);
            }
            break;
        case FAN_DOWN:
            faceDelta = cardHeight / CARD_FACE_FAN_FACTOR;
            backDelta = cardHeight / CARD_BACK_FAN_FACTOR;
            c = (struct Card*)ArrayFirst(self->cards, &index);
            while ( c ) {
                pos.y += c->prone ? backDelta : faceDelta;
                c = (struct Card*)ArrayNext(self->cards, &index);
            }
            break;
        case FAN_RIGHT3:
            // fprintf(stdout, "Waste Right before %.0f,%.0f\n", pos.x, pos.y);
            {
                float x0 = pos.x;
                float y0 = pos.y;
                float x1 = x0 + cardWidth / CARD_FACE_FAN_FACTOR;
                float x2 = x1 + cardWidth / CARD_FACE_FAN_FACTOR;
                switch ( ArrayLen(self->cards) ) {
                    case 0:
                        // do nothing, incoming card will be at pos
                        break;
                    case 1:
                        // incoming card at slot[1]
                        pos.x = x1;
                        break;
                    case 2:
                        // incoming card at slot[2]
                        pos.x = x2;
                        break;
                    default:
                        // >= 3 cards
                        // incoming card at slot[2]
                        pos.x = x2;
                        // top card needs to transition from slot[2] to slot[1]
                        int i = (int)ArrayLen(self->cards) - 1;
                        CardTransitionTo(ArrayGet(self->cards, i), (Vector2){.x=x1, .y=y0});
                        // mid card needs to transition from slot[1] to slot[0]
                        for ( --i; i>= 0; i-- ) {
                            CardTransitionTo(ArrayGet(self->cards, i), (Vector2){.x=x0, .y=y0});
                        }
                        break;
                }
            }
            // fprintf(stdout, "Waste Right after  %.0f,%.0f\n", pos.x, pos.y);
            break;
        case FAN_LEFT3:
            break;
        case FAN_DOWN3:
            {
                float x0 = pos.x;
                float y0 = pos.y;
                float y1 = y0 + cardHeight / CARD_FACE_FAN_FACTOR;
                float y2 = y1 + cardHeight / CARD_FACE_FAN_FACTOR;
                switch ( ArrayLen(self->cards) ) {
                    case 0:
                        // do nothing, incoming card will be at pos
                        break;
                    case 1:
                        // incoming card at slot[1]
                        pos.y = y1;
                        break;
                    case 2:
                        // incoming card at slot[2]
                        pos.y = y2;
                        break;
                    default:
                        // >= 3 cards
                        // incoming card at slot[2]
                        pos.y = y2;
                        // top card needs to transition from slot[2] to slot[1]
                        int i = (int)ArrayLen(self->cards) - 1;
                        CardTransitionTo(ArrayGet(self->cards, i), (Vector2){.x=x0, .y=y1});
                        // mid card needs to transition from slot[1] to slot[0]
                        for ( --i; i>= 0; i-- ) {
                            CardTransitionTo(ArrayGet(self->cards, i), (Vector2){.x=x0, .y=y0});
                        }
                        break;
                }
            }
            break;
        default:
            break;
    }
    return pos;
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

    // flip up an exposed source card, if src pile is not Stock*
    if ( !PileIsStock(src) ) {
        struct Card* tc = PilePeekCard(src);
        if ( tc ) {
            CardFlipUp(tc);
        }
    }

    // special case: waste may need refanning if we took a card from it
    if ( src->fanType == FAN_DOWN3 || src->fanType == FAN_LEFT3 || src->fanType == FAN_RIGHT3 ) {
        PileRepushAllCards(src);
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
    size_t oldSrcLen = PileLen(src);
#if 0
    // find the new length of the source pile
    // TODO could use ArrayIndexOf for this; index == newSrcLen?, retire pc
    size_t newSrcLen = 0, index;
    struct Card* pc = ArrayFirst(src->cards, &index);
    while (pc) {
        if (pc == c) {
            break;
        }
        newSrcLen++;
        pc = ArrayNext(src->cards, &index);
    }

    if (!pc) {
        fprintf(stderr, "ERROR: %s: could not find card in pile\n", __func__);
        return 0;
    }
#else
    size_t newSrcLen;
    if (!ArrayIndexOf(src->cards, c, &newSrcLen)) {
        fprintf(stderr, "ERROR: %s: could not find card in pile\n", __func__);
        return 0;
    }
#endif

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

    // flip up an exposed source card, if src pile is not Stock*
    // if ( strncmp(src->category, "Stock", 5) ) {
    if (!PileIsStock(src)) {
        struct Card* tc = PilePeekCard(src);
        if (tc) {
            CardFlipUp(tc);
        }
    }

    // special case: waste may need refanning if we took a card from it
    if ( src->fanType == FAN_DOWN3 || src->fanType == FAN_LEFT3 || src->fanType == FAN_RIGHT3 ) {
        PileRepushAllCards(src);
    }

    // TODO scrunch

    if (newSrcLen == oldSrcLen) {
        fprintf(stderr, "WARNING: %s did nothing?\n", __func__);
        return 0;
    }

    return newSrcLen != oldSrcLen;
}

void PileRepushAllCards(struct Pile *const self)
{
    if (PileEmpty(self)) {
        return;
    }
    struct Array *tmp = ArrayClone(self->cards);
    ArrayReset(self->cards);

    size_t index;
    for ( struct Card *c = ArrayFirst(tmp, &index); c; c = ArrayNext(tmp, &index) ) {
        PilePushCard(self, c);
    }
    ArrayFree(tmp);
}

void PileGenericTapped(struct Pile *const self, struct Array *const tail)
{
    struct Baize* baize = self->owner;
    size_t index;
    for ( struct Pile* fp = ArrayFirst(baize->foundations, &index); fp; fp = ArrayNext(baize->foundations, &index) ) {
        if ( fp->vtable->CanAcceptTail(baize, fp, tail) ) {
            struct Card *c = ArrayGet(tail, 0);
            PileMoveCard(fp, c->owner);
            break;
        }
    }
}

int PileGenericCollect(struct Pile *const self)
{
    // Collect is like tapping on the top card of each pile (except Stock), or on a K in a Spider pile
    // prefer to collect a run of cards from one pile to one foundation

    // NB Spider piles are not collected because moving them to the 'foundations' is optional according to Morehead and Mott-Smith
    // so Spider games have Discard piles, not Foundation piles
    // Spider could be complete when a Tableau is either empty or contains 13 conformant cards (TODO not currently implemented)
    struct Baize* baize = self->owner;
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
