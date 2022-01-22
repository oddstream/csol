/* tableau.c */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <raylib.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "tableau.h"
#include "trace.h"
#include "constraint.h"

static struct PileVtable tableauVtable = {
    &TableauCanMoveTail,
    &TableauCanAcceptCard,
    &TableauCanAcceptTail,
    &GenericTailTapped,
    &TableauCollect,
    &TableauComplete,
    &TableauUnsortedPairs,

    &PileReset,
    &PileUpdate,
    &PileDraw,
    &PileFree,
};

struct Tableau* TableauNew(struct Baize *const baize, Vector2 slot, enum FanType fan, enum MoveType move)
{
    struct Tableau* self = calloc(1, sizeof(struct Tableau));
    if (self) {
        PileCtor(baize, (struct Pile*)self, "Tableau", slot, fan);
        self->super.vtable = &tableauVtable;
        self->moveType = move;
    }
    return self;
}

_Bool TableauCanMoveTail(struct Array *const tail)
{
    if (!tail) {
        CSOL_ERROR("%s", "invalid tail");
        return 0;
    }

    struct Card *c0 = ArrayGet(tail, 0);
    if (!CardValid(c0)) {
        CSOL_ERROR("%s", "invalid card");
        return 0;
    }

    struct Pile *pile = CardOwner(c0);
    struct Baize *baize = PileOwner(pile);
    struct Tableau *tab = (struct Tableau*)pile;
    switch (tab->moveType) {
    case MOVE_ANY:
        break;
    case MOVE_ONE:
        if (ArrayLen(tail) > 1) {
            BaizeSetError(baize, "(CSOL) You can only move one card");
            return 0;
        }
    case MOVE_ONE_PLUS:
        // don't know destination, so we allow this as MOVE_ANY
        break;
    case MOVE_ONE_OR_ALL:
        if (ArrayLen(tail)==1) {
            // that's ok
        } else if (ArrayLen(tail) == PileLen(pile)) {
            // that's ok too
        } else {
            BaizeSetError(baize, "(CSOL) Only move one card, or the whole pile");
            return 0;
        }
        break;
    default:
        break;
    }
    return CanTailBeMoved(tail);
}

_Bool TableauCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    (void)baize;

    struct Array1 tail = Array1New(c);
    return CanTailBeAppended(self, (struct Array*)&tail);
    // don't need to free an Array1
}

static size_t PowerMoves(struct Baize *const self, struct Pile *const dstPile)
{
    double emptyCells = 0.0;
    double emptyCols = 0.0;
    size_t index;
    for ( struct Pile *p=ArrayFirst(self->piles, &index); p; p=ArrayNext(self->piles, &index) ) {
        if ( ArrayLen(p->cards) == 0 ) {
            if ( strcmp(p->category, "Cell") == 0 ) {
                emptyCells++;
            } else if ( strcmp(p->category, "Tableau") == 0 ) {
                // 'If you are moving into an empty column, then the column you are moving into does not count as empty column.'
                if ( p->label[0] == '\0' && p != dstPile ) {
                    emptyCols++;
                }
            }
        }
    }
    double n = (1.0 + emptyCells) * pow(2.0, emptyCols);
    return (size_t)n;
}

_Bool TableauCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    if (ArrayLen(tail) == 1) {
        return TableauCanAcceptCard(baize, self, ArrayGet(tail, 0));
    }
    // we know the tail can be moved (else, how did we get here?)
    // so just need to check power moves here
    struct Tableau *tab = (struct Tableau*)self;
    if (tab->moveType == MOVE_ONE_PLUS) {
        size_t moves = PowerMoves(baize, self);
        if ( ArrayLen(tail) > moves ) {
            char z[128];
            if ( moves == 1 )
                sprintf(z, "(CSOL) Only enough space to move 1 card, not %lu", ArrayLen(tail));
            else
                sprintf(z, "(CSOL) Only enough space to move %lu cards, not %lu", moves, ArrayLen(tail));
            BaizeSetError(baize, z);
            return 0;
        }
    }
    return CanTailBeAppended(self, tail);
}

int TableauCollect(struct Pile *const self)
{
    return PileGenericCollect(self);
}

_Bool TableauComplete(struct Pile *const self)
{   /*
        'complete' means
            (a) empty
            (b) if discard piles exist, then there are no unsorted card pairs
                and pile len == baize->numberOfCardsInLibrary / ndiscards
                there will always be one discard pile for each suit
                (number of discard piles is packs * suits)
                (Simple Simon has 10 tableau piles and 4 discard piles)
                (Spider has 10 tableau piles and 8 discard piles)
    */
    if (PileEmpty(self)) {
        return 1;
    }
    struct Baize *baize = PileOwner(self);
    int ndiscards = ArrayLen(baize->discards);
    if (ndiscards) {
        return (PileLen(self) == baize->numberOfCardsInLibrary / ndiscards) && (TableauUnsortedPairs(self) == 0);
    }
    return 0;
}

int TableauUnsortedPairs(struct Pile *const self)
{
    if (PileLen(self) > 1) {
        struct Baize *const baize = PileOwner(self);
        return baize->script->PileUnsortedPairs(self);
    } else {
        return 0;
    }
}
