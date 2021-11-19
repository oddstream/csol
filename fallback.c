/* fallback.c */

#include <string.h>

#include "exiface.h"

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

    for ( int x = 6; x < 10; x++ ) {
        pile = (struct Pile*)FoundationNew(baize, (Vector2){x, 0}, FAN_NONE);
        baize->piles = ArrayPush(baize->piles, pile);
        baize->foundations = ArrayPush(baize->foundations, pile);
        pile->vtable->SetLabel(pile, "A");
    }

    for ( int x = 0; x < 10; x++ ) {
        pile = (struct Pile*)TableauNew(baize, (Vector2){x, 1}, FAN_DOWN, MOVE_ANY);
        baize->piles = ArrayPush(baize->piles, pile);
        baize->tableaux = ArrayPush(baize->tableaux, pile);
    }
}

static void StartGame(struct Baize *const baize)
{
    size_t index;
    for ( struct Pile *pile = ArrayFirst(baize->tableaux, &index); pile; pile = ArrayNext(baize->tableaux, &index)) {
        for ( int i=0; i<2; i++ ) {
            PileMoveCard(pile, baize->stock);
            CardFlipDown(PilePeekCard(pile));
        }
    }
    for ( struct Pile *pile = ArrayFirst(baize->tableaux, &index); pile; pile = ArrayNext(baize->tableaux, &index)) {
        PileMoveCard(pile, baize->stock);
    }
    baize->stock->vtable->SetRecycles(baize->stock, 1);
}

static void AfterMove(struct Baize *const baize)
{
    (void)baize;
}

static const char* TailMoveError(struct Array *const tail)
{
    (void)tail;
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
            return (void*)0;
        } else {
            struct Card *c1 = PilePeekCard(pile);
            struct Card *c2 = ArrayGet(tail, 0);
            return CardCompare_DownSuit(c1, c2);
        }
    } else if (strcmp(pile->category, "Waste")==0) {
        return "Waste can only accept cards from Stock";
    }
    return (void*)0;
}

static const char* PileConformantError(struct Pile *const pile)
{
    const char *strerr = (void*)0;
    struct Card *c1 = ArrayGet(pile->cards, 0);
    struct Card *c2;
    for ( size_t i=1; i<ArrayLen(pile->cards); i++) {
        c2 = ArrayGet(pile->cards, i);
        if ((strerr = CardCompare_DownSuit(c1, c2)))
            break;
        c1 = c2;
    }
    return strerr;
}

static void PileSortedAndUnsorted(struct Pile *const pile, int* sorted, int* unsorted)
{
    const char *strerr = (void*)0;
    struct Card *c1 = ArrayGet(pile->cards, 0);
    struct Card *c2;
    for ( size_t i=1; i<ArrayLen(pile->cards); i++) {
        c2 = ArrayGet(pile->cards, i);
        if ((strerr = CardCompare_DownSuit(c1, c2)))
            *unsorted = *unsorted + 1;
        else
            *sorted = *sorted + 1;
        c1 = c2;
    }
}

static void TailTapped(struct Array *const tail)
{
    struct Card *c1 = ArrayGet(tail, 0);
    struct Pile *pile = CardOwner(c1);
    struct Baize *baize = PileOwner(pile);
    if (pile == baize->stock) {
        if (ArrayLen(baize->stock->cards) > 0) {
            PileMoveCard(baize->waste, baize->stock);
        }
    } else {
        pile->vtable->TailTapped(pile, tail);
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
    } else {
        pile->vtable->PileTapped(pile);
    }
}

static int PercentComplete(struct Baize *const baize)
{
    int percent = 0, sorted = 0, unsorted = 0;
    size_t index;
    for ( struct Pile *pile = ArrayFirst(baize->piles, &index); pile; pile = ArrayNext(baize->piles, &index) ) {
        pile->vtable->CountSortedAndUnsorted(pile, &sorted, &unsorted);
    }
    percent = (int)(UtilMapValue((float)sorted-(float)unsorted, -(float)baize->numberOfCardsInLibrary, (float)baize->numberOfCardsInLibrary, 0.0f, 100.0f));
    return percent;
}

static struct ExecutionInterface fallbackVtable = {
    &BuildPiles,
    &StartGame,
    &AfterMove,
    &TailMoveError,
    &TailAppendError,
    &PileConformantError,
    &PileSortedAndUnsorted,
    &TailTapped,
    &PileTapped,
    &PercentComplete,
};

struct ExecutionInterface* GetFallbackInterface(void)
{
    return &fallbackVtable;
}