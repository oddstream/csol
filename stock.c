/* stock.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <raylib.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "moon.h"
#include "stock.h"

static struct PileVtable stockVtable = {
    &StockCanAcceptCard,
    &StockCanAcceptTail,
    &StockCollect,
    &StockComplete,
    &StockConformant,
    &StockSetAccept,
    &StockSetRecycles,
    &StockCountSortedAndUnsorted,

    &PileUpdate,
    &StockDraw,
    &PileFree,
};

static void ParseCardFilter(lua_State *L, bool cardFilter[14])
{
    if ( lua_getglobal(L, "STRIP_CARDS") != LUA_TTABLE ) {
        fprintf(stdout, "STRIP_CARDS is not set\n");
    } else {
        fprintf(stdout, "STRIP_CARDS is set\n");
        for ( int i=1; i<14; i++ ) {
            lua_pushinteger(L, i);    // pushes +1 onto stack
            lua_gettable(L, -2);      // pops integer/index, pushes STRIP_CARDS[i]
            if ( lua_isnumber(L, -1) ) {
                int result;
                result = lua_tointeger(L, -1);    // doesn't alter stack
                if ( result > 0 && result < 14 ) {
                    cardFilter[result] = 0;
                } else {
                    fprintf(stderr, "ERROR: STRIP_CARDS: invalid value %d\n", result);
                }
            }
            lua_pop(L, 1);    // remove result of lua_gettable
        }
    }
    lua_pop(L, 1);    // remove result of lua_getglobal
}

static void CreateCardLibrary(struct Baize *const baize)
{
    bool cardFilter[14] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    ParseCardFilter(baize->L, cardFilter);
    baize->numberOfCardsInSuit = 0;
    for ( int i=1; i<14; i++ ) {
        if (cardFilter[i]) {
            baize->numberOfCardsInSuit += 1;
        }
    }

    // TODO get PACKS, SUITS, STRIP_CARDS inside Moon:AddPile and pass them as parameters to StockNew
    size_t packs = MoonGetGlobalInt(baize->L, "PACKS", 1);
    size_t suits = MoonGetGlobalInt(baize->L, "SUITS", 4);
    size_t cardsRequired = packs * suits * baize->numberOfCardsInSuit;

    // first time this is called, baize->cardLibrary will be NULL because we used calloc()
    if (baize->cardLibrary) {
        free(baize->cardLibrary);
    }
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
    baize->numberOfCardsInLibrary = i;   // incase any were taken out by cardFilter

    fprintf(stdout, "%s: packs=%lu, suits=%lu, cards created=%lu\n", __func__, packs, suits, baize->numberOfCardsInLibrary);
}

static void FillStockFromLibrary(struct Baize *const baize, struct Pile *const stock)
{
    for ( size_t i = 0; i < baize->numberOfCardsInLibrary; i++ ) {
        PilePushCard(stock, &baize->cardLibrary[i]);
    }
}

static void ShuffleStock(struct Baize *const baize, struct Pile *const stock)
{
    unsigned int seed = MoonGetGlobalInt(baize->L, "SEED", time(NULL) & 0xFFFF);
    srand(seed);
    // Knuth-Fisher–Yates shuffle
    size_t n = ArrayLen(stock->cards);
    for ( int i = n-1; i > 0; i-- ) {
        int j = rand() % (i+1);
        ArraySwap(stock->cards, i, j);
    }
}

struct Stock* StockNew(struct Baize *const baize, Vector2 slot, enum FanType fan)
{
    struct Stock* self = calloc(1, sizeof(struct Stock));
    if ( self ) {
        PileCtor(baize, (struct Pile*)self, "Stock", slot, fan);
        self->super.vtable = &stockVtable;

        CreateCardLibrary(baize);
        FillStockFromLibrary(baize, (struct Pile *const)self);
        ShuffleStock(baize, (struct Pile *const)self);

        self->recycles = 2147483647;  // infinite by default
    }
    return self;
}

bool StockCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    BaizeSetError(baize, "(C) You cannot move cards to the Stock");

    (void)baize;
    (void)self;
    (void)c;
    return false;
}

bool StockCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    BaizeSetError(baize, "(C) You cannot move cards to the Stock");

    (void)baize;
    (void)self;
    (void)tail;
    return false;
}

int StockCollect(struct Pile *const self)
{
    (void)self;
    return 0;
}

bool StockComplete(struct Pile *const self)
{
    return PileEmpty(self);
}

bool StockConformant(struct Pile *const self)
{
    return PileEmpty(self);
}

void StockSetAccept(struct Pile *const self, enum CardOrdinal ord)
{
    (void)self;
    (void)ord;
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

    PileDraw(self);

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
}
