/* fallback.c */

#include <string.h>

#include "script.h"

#include "pile.h"
#include "foundation.h"
#include "stock.h"
#include "tableau.h"
#include "waste.h"

static void BuildPiles(struct Baize *const baize)
{
    _Bool cardFilter[14] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    struct Pile *pile;

    baize->stock = (struct Pile*)StockNew(baize, (Vector2){0, 0}, FAN_NONE, 1, 4, cardFilter);
    baize->piles = ArrayPush(baize->piles, baize->stock);

    baize->waste = (struct Pile*)WasteNew(baize, (Vector2){1, 0}, FAN_RIGHT3);
    baize->piles = ArrayPush(baize->piles, baize->waste);

    for ( int x = 3; x < 7; x++ ) {
        pile = (struct Pile*)FoundationNew(baize, (Vector2){x, 0}, FAN_NONE);
        baize->piles = ArrayPush(baize->piles, pile);
        baize->foundations = ArrayPush(baize->foundations, pile);
        PileSetLabel(pile, "A");
    }

    for ( int x = 0; x < 7; x++ ) {
        pile = (struct Pile*)TableauNew(baize, (Vector2){x, 1}, FAN_DOWN, MOVE_ANY);
        baize->piles = ArrayPush(baize->piles, pile);
        baize->tableaux = ArrayPush(baize->tableaux, pile);
        PileSetLabel(pile, "K");
    }
}

static void StartGame(struct Baize *const baize)
{
    int dealDown = 0;
    size_t index;
    for ( struct Pile *pile = ArrayFirst(baize->tableaux, &index); pile; pile = ArrayNext(baize->tableaux, &index)) {
        for ( int i=0; i<dealDown; i++ ) {
            PileMoveCard(pile, baize->stock);
            CardFlipDown(PilePeekCard(pile));
        }
        dealDown++;
        PileMoveCard(pile, baize->stock);
    }
    PileMoveCard(baize->waste, baize->stock);
    ((struct Stock*)baize->stock)->recycles = 32767;
}

static void AfterMove(struct Baize *const baize)
{
    if ( PileEmpty(baize->waste) && !PileEmpty(baize->stock) ) {
        PileMoveCard(baize->waste, baize->stock);
    }
}

static const char* TailMoveError(struct Array *const tail)
{
    struct Card *c1 = ArrayGet(tail, 0);
    struct Pile *pile = CardOwner(c1);
    if (strcmp(pile->category, "Tableau")) {
        const char *strerr = (void*)0;
        struct Card *c2;
        for ( size_t i=1; i<ArrayLen(tail); i++) {
            c2 = ArrayGet(tail, i);
            if ((strerr = CardCompare_DownAltColor(c1, c2))) {
                return strerr;
            }
            c1 = c2;
        }
    }
    return (void*)0;
}

static const char* TailAppendError(struct Pile *const pile, struct Array *const tail)
{
    if (strcmp(pile->category, "Foundation")==0) {
        if (PileEmpty(pile)) {
            struct Card *c1 = ArrayGet(tail, 0);
            if (CardOrdinal(c1) != 1) {
                return "Foundation can only accept an Ace";
            }
        } else {
            struct Card *c1 = PilePeekCard(pile);
            struct Card *c2 = ArrayGet(tail, 0);
            return CardCompare_UpSuit(c1, c2);
        }
    } else if (strcmp(pile->category, "Tableau")==0) {
        if (PileEmpty(pile)) {
            struct Card *c1 = ArrayGet(tail, 0);
            if (CardOrdinal(c1) != 13) {
                return "Tableau can only accept an King";
            }
        } else {
            struct Card *c1 = PilePeekCard(pile);
            struct Card *c2 = ArrayGet(tail, 0);
            return CardCompare_DownAltColor(c1, c2);
        }
    }
    return (void*)0;
}

static int PileUnsortedPairs(struct Pile *const pile)
{
    int unsorted = 0;
    const char *strerr = (void*)0;
    struct Card *c1 = ArrayGet(pile->cards, 0);
    struct Card *c2;
    for ( size_t i=1; i<ArrayLen(pile->cards); i++) {
        c2 = ArrayGet(pile->cards, i);
        if ((strerr = CardCompare_DownAltColor(c1, c2))) {
            unsorted = unsorted + 1;
        }
        c1 = c2;
    }
    return unsorted;
}

static void TailTapped(struct Array *const tail)
{
    struct Card *c1 = ArrayGet(tail, 0);
    struct Pile *pile = CardOwner(c1);
    struct Baize *baize = PileOwner(pile);
    if (strcmp(pile->category, "Stock") == 0) {
        if (ArrayLen(baize->stock->cards) > 0) {
            PileMoveCard(baize->waste, baize->stock);
        }
    } else {
        pile->vtable->TailTapped(tail);
    }
}

static void PileTapped(struct Pile *const pile)
{
    struct Baize *baize = PileOwner(pile);
    if (pile == baize->stock) {
        struct Stock *s = (struct Stock*)pile;
        if (s->recycles > 0) {
            while (ArrayLen(baize->waste->cards) > 0) {
                PileMoveCard(baize->stock, baize->waste);
            }
            s->recycles -= 1;
        }
    }
}

static int PercentComplete(struct Baize *const baize)
{
    int percent = 0, pairs = 0, unsorted = 0;
    size_t index;
    for ( struct Pile *pile = ArrayFirst(baize->piles, &index); pile; pile = ArrayNext(baize->piles, &index) ) {
        if (PileLen(pile) > 1) {
            pairs += PileLen(pile) - 1;
        }
        unsorted += pile->vtable->UnsortedPairs(pile);
    }
    percent = (int)(100.0f - UtilMapValue((float)unsorted, 0, (float)pairs, 0.0f, 100.0f));
    return percent;
}

static const char* Wikipedia(void)
{
    return "https://en.wikipedia.org/wiki/Solitaire";
}

static struct ScriptInterface clondikeVtable = {
    &BuildPiles,
    &StartGame,
    &AfterMove,
    &TailMoveError,
    &TailAppendError,
    &PileUnsortedPairs,
    &TailTapped,
    &PileTapped,
    &PercentComplete,
    &Wikipedia,
};

struct ScriptInterface* GetClondikeInterface(void)
{
    return &clondikeVtable;
}