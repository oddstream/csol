/* pilevtable.c */

#include <stdio.h>
#include <string.h>

#include "array.h"
#include "baize.h"
#include "card.h"
#include "pile.h"
#include "scrunch.h"
#include "trace.h"

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

void InertTailTapped(struct Array *const tail)
{
    (void)tail;
}

void GenericTailTapped(struct Array *const tail)
{
    struct Card *card = ArrayGet(tail, 0);
    struct Pile *pile = CardOwner(card);
    struct Baize *baize = PileOwner(pile);
    size_t index;

    if ( ArrayLen(tail) == 1 ) {
        // can only send single cards to Foundation piles
        for ( struct Pile* fp = ArrayFirst(baize->foundations, &index); fp; fp = ArrayNext(baize->foundations, &index) ) {
            if ( fp->vtable->CanAcceptCard(baize, fp, card) ) {
                if ( PileMoveCard(fp, pile) ) {
                    return;
                }
            }
        }
    }

    struct Pile *chosenPile = (void*)0;
    for ( struct Pile* tp = ArrayFirst(baize->tableaux, &index); tp; tp = ArrayNext(baize->tableaux, &index) ) {
        if ( tp == pile )
            continue;
        if ( PileEmpty(tp) && tp->label[0] == '\0' )
            continue;
        if ( pile->vtable->CanMoveTail(baize->tail) ) {
            if ( tp->vtable->CanAcceptTail(baize, tp, baize->tail) ) {
                // CSOL_INFO("pile %s can accept tail", tp->category);
                struct Card *tc = PilePeekCard(tp);
                if ( (tc != NULL) && (card->id.suit == tc->id.suit) ) {
                    chosenPile = tp;
                    break;
                }
                if ( chosenPile == NULL || PileLen(tp) < PileLen(chosenPile) ) {
                    chosenPile = tp;
                }
            } else {
                // CSOL_INFO("pile %s cannot accept tail", tp->category);
            }
        }
    }
    if ( chosenPile ) {
        // CSOL_INFO("pile %s will accept tail", chosenPile->category);
        PileMoveCards(chosenPile, card);
        // don't call BaizeAfterUserMove(baize);
        // need to undo push is handled by caller
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

int PileGenericUnsortedPairs(struct Pile *const self)
{
    if (PileEmpty(self)) {
        return 0;
    }
    return PileLen(self) - 1;
}

int PileInertUnsortedPairs(struct Pile *const self)
{
    (void)self;
    return 0;
}

void PileReset(struct Pile *const self)
{
    ArrayReset(self->cards);
}

void PileUpdate(struct Pile *const self)
{
    ArrayForeach(self->cards, (ArrayIterFunc)CardUpdate);
}

void PileDraw(struct Pile *const self)
{
    extern Color baizeHighlightColor;

    struct Baize *baize = PileOwner(self);
    struct Pack *pack = baize->pack;

    // BeginDrawing() has been called by BaizeDraw()
    // Rectangle r = PileFannedScreenRect(self);
    Rectangle r = PileScreenRect(self);
    DrawRectangleRoundedLines(r, pack->roundness, 9, 2.0f, baizeHighlightColor);

    PileDrawCenteredText(self, self->label);

#ifdef _DEBUG
    // ScrunchDrawDebug(self);
#endif
}

void PileFree(struct Pile *const self)
{
    // Card objects exist in the Baize->cardLibrary array, so we don't free them here
    if (self) {
        ArrayFree(self->cards);
        self->magic = 0;
        free(self);
    }
}
