/* tableau.c */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <raylib.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "tableau.h"
#include "constraint.h"
#include "util.h"

static struct PileVtable tableauVtable = {
    &TableauCanAcceptCard,
    &TableauCanAcceptTail,
    &TableauCollect,
    &TableauComplete,
    &TableauConformant,
    &TableauSetAccept,
    &TableauSetRecycles,
    &TableauCountSortedAndUnsorted,

    &PileUpdate,
    &TableauDraw,
    &PileFree,
};

struct Tableau* TableauNew(Vector2 slot, enum FanType fan)
{
    struct Tableau* self = calloc(1, sizeof(struct Tableau));
    if ( self ) {
        PileCtor((struct Pile*)self, "Tableau", slot, fan);
        self->super.vtable = &tableauVtable;
        self->accept = 0;   // accept any by default
    }
    return self;
}

bool TableauCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    (void)baize;    // TODO retire
    bool result = false;
    struct Array *tail = ArrayNew(1);
    if (tail) {
        ArrayPush(tail, c);
        // TODO check card draggable?
        result = CanTailBeAppended(self, tail);
        ArrayFree(tail);
    }
    return result;
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
                struct Tableau *t = (struct Tableau*)p;
                if ( t->accept == 0 && p != dstPile ) {
                    emptyCols++;
                }
            }
        }
    }
    double n = (1.0 + emptyCells) * pow(2.0, emptyCols);
    return (size_t)n;
}

bool TableauCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    if ( ArrayLen(tail) > 1 ) {
        if ( baize->powerMoves ) {
            size_t moves = PowerMoves(baize, self);
            if ( ArrayLen(tail) > moves ) {
                char z[128];
                if ( moves == 1 )
                    sprintf(z, "(C) Only enough space to move 1 card, not %lu", ArrayLen(tail));
                else
                    sprintf(z, "(C) Only enough space to move %lu cards, not %lu", moves, ArrayLen(tail));
                BaizeSetError(baize, z);
                return false;
            }
        }
    }
    if ( !CanTailBeMoved(tail) ) {
        return false;
    }
    return CanTailBeAppended(self, tail);
}

int TableauCollect(struct Pile *const self)
{
    return PileGenericCollect(self);
}

bool TableauComplete(struct Pile *const self)
{
    return PileEmpty(self);
}

bool TableauConformant(struct Pile *const self)
{
    return IsPileConformant(self);
}

void TableauSetAccept(struct Pile *const self, enum CardOrdinal ord)
{
    ((struct Tableau*)self)->accept = ord;
}

void TableauSetRecycles(struct Pile *const self, int r)
{
    // we don't do that here
    (void)self;
    (void)r;
}

void TableauCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
#if 1
    (void)self;
    (void)sorted;
    (void)unsorted;
#else
    // iterate the pile->cards using an unsigned size_t, so make sure there are >2 before doing len-1
    if (PileEmpty(self)) {
        return;
    }
    if ( ArrayLen(self->cards) == 1 ) {
        *sorted += 1;
        return;
    }
    for ( size_t i = 0; i<ArrayLen(self->cards) - 1; i++ ) {
        struct Card *c0 = ArrayGet(self->cards, i);
        if ( !CardValid(c0) ) {
            fprintf(stderr, "WARNING: Invalid card 0 in %s\n", __func__);
        }
        struct Card *c1 = ArrayGet(self->cards, i+1);
        if ( !CardValid(c1) ) {
            fprintf(stderr, "WARNING: Invalid card 1 in %s\n", __func__);
        }
        if ( c0->prone || c1->prone ) {
            *unsorted += 1;
        } else {
            if ( CheckPair(self->owner, c0, c1) ) {
                *sorted += 1;
            } else {
                *unsorted += 1;
            }
        }
    }
#endif
}

void TableauDraw(struct Pile *const self)
{
    extern Font fontAcme;
    extern Color baizeHighlightColor;

    PileDraw(self);

    struct Tableau* t = (struct Tableau*)self;
    if ( t->accept != 0 ) {
        extern float cardWidth;
        float fontSize = cardWidth / 2.0f;
        Vector2 pos = PileScreenPos(self);
        pos.x += cardWidth / 8.0f;
        pos.y += cardWidth / 16.0f;
        DrawTextEx(fontAcme, UtilOrdToShortString(t->accept), pos, fontSize, 0, baizeHighlightColor);
    }
}
