/* stock.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <raylib.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "luautil.h"
#include "stock.h"

static struct PileVtable stockVtable = {
    &StockCanMoveTail,
    &PileInertCanMatchTail,
    &StockCanAcceptCard,
    &StockCanAcceptTail,
    &PileInertTapped,       // tapping the stock pile to recycle is now handled by Lua
                            // this function is a fallback in case there is no function Stock.Tapped
    &PileInertCollect,
    &StockComplete,
    &StockConformant,
    &StockSetRecycles,
    &StockCountSortedAndUnsorted,

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
                if ( cardFilter[o] ) {
                    baize->cardLibrary[i++] = CardNew(pack, o, s);
                } else {
                    // fprintf(stderr, "Skipping %u\n", o);
                }
            }
        }
    }
    baize->numberOfCardsInLibrary = i;   // in case any were taken out by cardFilter

#ifdef _DEBUG
    fprintf(stdout, "%s: packs=%lu, suits=%lu, cards created=%lu\n", __func__, packs, suits, baize->numberOfCardsInLibrary);
#endif
}

static void FillStockFromLibrary(struct Baize *const baize, struct Pile *const stock)
{
    for ( size_t i = 0; i < baize->numberOfCardsInLibrary; i++ ) {
        PilePushCard(stock, &baize->cardLibrary[i]);
    }
}

static void ShuffleStock(struct Baize *const baize, struct Pile *const stock)
{
    unsigned seed = LuaUtilGetGlobalInt(baize->L, "SEED", time(NULL) & 0xFFFF);
#ifdef _DEBUG
    fprintf(stdout, "SEED %u\n", seed);
#endif
    srand(seed);
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
        ShuffleStock(baize, (struct Pile *const)self);

        self->recycles = 32767;  // infinite by default
    }
    return self;
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

_Bool StockConformant(struct Pile *const self)
{
    return PileEmpty(self);
}

void StockSetRecycles(struct Pile *const self, int r)
{
    struct Stock *s = (struct Stock *)self;
    s->recycles = r;
}

void StockCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
    (void)sorted;
    *unsorted += ArrayLen(self->cards);
}

void StockDraw(struct Pile *const self)
{
    extern float cardWidth, cardHeight;
    extern Texture2D recycleTexture;
    extern Color baizeHighlightColor;
    // extern Font fontAcme;

    // icon is 96x96
    // card is, say, 100x120

    struct Stock* s = (struct Stock*)self;
    if (s->recycles) {
        float iconSize = 96.0f;
        float iconScale = 1.0f;
        // retro cards are 71 wide, kenney cards are 140 wide
        if (iconSize >= cardWidth) {
            iconScale = cardWidth / 96.0f;  // eg 71/96=0.7395
            iconSize *= iconScale;
        }
        Vector2 pos = PileScreenPos(self);
        pos.x += (cardWidth - iconSize) / 2.0f;
        pos.y += (cardHeight - iconSize) / 2.0f;
        if ( CheckCollisionPointRec(GetMousePosition(), (Rectangle){.x=pos.x, .y=pos.y, .width=iconSize, .height=iconSize}) ) {
            pos.x += 2.0f;
            pos.y += 2.0f;
        }
        DrawTextureEx(
            recycleTexture,
            pos,
            0.0f,   // rotation
            iconScale,   // scale
            baizeHighlightColor
        );
        // if (s->recycles<10) {
        //     char str[16]; sprintf(str, "%d", s->recycles);
        //     Vector2 mte = MeasureTextEx(fontAcme, str, 24.0f, 0.0f);
        //     pos = PileScreenPos(self);
        //     pos.x += (cardWidth - mte.x) / 2.0f;
        //     pos.y += (cardHeight - mte.y) / 2.0f;
        //     DrawTextEx(fontAcme, str, pos, 24.0f, 0.0f, baizeHighlightColor);
        // }
    }

    PileDraw(self);
}
