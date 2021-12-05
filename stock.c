/* stock.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <raylib.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "stock.h"
#include "trace.h"
#include "ui.h"

static struct PileVtable stockVtable = {
    &StockCanMoveTail,
    &StockCanAcceptCard,
    &StockCanAcceptTail,
    &CardInertTapped,
    &PileInertCollect,
    &StockComplete,
    &PileGenericUnsortedPairs,

    &StockReset,
    &PileUpdate,
    &StockDraw,
    &PileFree,
};

static void CreateCardLibrary(struct Baize *const baize, size_t packs, size_t suits, _Bool cardFilter[14])
{
    // first time this is called, baize->cardLibrary will be NULL because we used calloc()
    if (baize->cardLibrary) {
        free(baize->cardLibrary);
    }

    size_t numberOfCardsInSuit = 0;
    for ( int i=1; i<14; i++ ) {
        if (cardFilter[i]) {
            numberOfCardsInSuit += 1;
        }
    }
    size_t cardsRequired = packs * suits * numberOfCardsInSuit;
    baize->cardLibrary = calloc(cardsRequired, sizeof(struct Card));
    baize->numberOfCardsInLibrary = cardsRequired;

    size_t i = 0;
    for ( size_t pack = 0; pack < packs; pack++ ) {
        for ( enum CardOrdinal o = ACE; o <= KING; o++ ) {
            for ( enum CardSuit s = 0; s < suits; s++ ) {
                if (cardFilter[o]) {
                    // run the suits loop backwards, so spades are used first
                    // (folks expect Spider One Suit to use spades)
                    baize->cardLibrary[i++] = CardNew(pack, o, SPADE - s);
                } else {
                    // fprintf(stderr, "Skipping %u\n", o);
                }
            }
        }
    }
    baize->numberOfCardsInLibrary = i;   // in case any were taken out by cardFilter

    CSOL_INFO("%lu packs, %lu suits, %lu cards created", packs, suits, baize->numberOfCardsInLibrary);
}

static void FillStockFromLibrary(struct Baize *const baize, struct Pile *const stock)
{
    for ( size_t i = 0; i < baize->numberOfCardsInLibrary; i++ ) {
        // don't use PilePushCard because baize->stock is not set up,
        // and we aren't fanning
        // PilePushCard(stock, &baize->cardLibrary[i]);
        struct Card *c = &baize->cardLibrary[i];
        // cards are created face down, but may be face up when recalled with StockReset
        c->prone = 1;
        c->owner = stock;
        c->pos = stock->pos;
        stock->cards = ArrayPush(stock->cards, c);
    }
}

static void ShuffleStock(struct Pile *const stock)
{
    extern int flag_noshuf;

    if (flag_noshuf) return;

    srand(time(NULL));
    // Knuth-Fisher–Yates shuffle
    size_t n = ArrayLen(stock->cards);
    for ( int i = n-1; i > 0; i-- ) {
        int j = rand() % (i+1);
        ArraySwap(stock->cards, i, j);
    }
}

struct Stock* StockNew(struct Baize *const baize, Vector2 slot, enum FanType fan, size_t packs, size_t suits, _Bool cardFilter[14])
{
    struct Stock* self = calloc(1, sizeof(struct Stock));
    if ( self ) {
        PileCtor(baize, (struct Pile*)self, "Stock", slot, fan);
        self->super.vtable = &stockVtable;

        CreateCardLibrary(baize, packs, suits, cardFilter);
        FillStockFromLibrary(baize, (struct Pile *const)self);
        ShuffleStock((struct Pile *const)self);

        self->recycles = 32767;  // infinite by default
    }
    return self;
}

void StockReset(struct Pile *const self)
{
    struct Baize *baize = PileOwner(self);
    ArrayReset(self->cards);
    FillStockFromLibrary(baize, self);
    ShuffleStock(self);
}

_Bool StockCanMoveTail(struct Array *const tail)
{
    (void)tail;
    return 1;
}

_Bool StockCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    BaizeSetError(baize, "(CSOL) Cannot move cards to the Stock");

    (void)baize;
    (void)self;
    (void)c;
    return 0;
}

_Bool StockCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    BaizeSetError(baize, "(CSOL) Cannot move cards to the Stock");

    (void)baize;
    (void)self;
    (void)tail;
    return 0;
}

_Bool StockComplete(struct Pile *const self)
{
    return PileEmpty(self);
}

void StockDraw(struct Pile *const self)
{
    struct Stock* s = (struct Stock*)self;
    if (s->recycles) {
        PileDrawCenteredGlyph(self, 0x267b);
    }

    PileDraw(self);
}
