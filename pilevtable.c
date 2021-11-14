/* pilevtable.c */

#include <stdio.h>
#include <string.h>

#include "array.h"
#include "baize.h"
#include "card.h"
#include "pile.h"
#include "scrunch.h"

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
    struct Baize *baize = PileOwner(self);
    struct Pack *pack = baize->pack;

    extern float fontSpacing;

    // fprintf(stdout, "INFO: %s: set %s label to '%s'\n", __func__, self->category, label);

    if (pack->pileFontSize==0) fprintf(stderr, "ERROR: %s: pileFontSize is zero\n", __func__);
    memset(self->label, 0, MAX_PILE_LABEL + 1);
    if (*label) {
        strncpy(self->label, label, MAX_PILE_LABEL);
        self->labelmte = MeasureTextEx(pack->fontAcmePile, self->label, (float)pack->pileFontSize, fontSpacing);
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

    struct Baize *baize = PileOwner(self);
    struct Pack *pack = baize->pack;

    // BeginDrawing() has been called by BaizeDraw()
    // Rectangle r = PileFannedScreenRect(self);
    Rectangle r = PileScreenRect(self);
    DrawRectangleRoundedLines(r, pack->roundness, 9, 2.0f, baizeHighlightColor);

    PileDrawCenteredText(self, self->label);

#if _DEBUG
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