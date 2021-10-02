/* tableau.c */

#include <stdlib.h>
#include <string.h>
#include <raylib.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "tableau.h"
#include "conformant.h"
#include "util.h"

static struct PileVtable tableauVtable = {
    &TableauCardTapped,
    &TableauPileTapped,
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

bool TableauCardTapped(struct Card *c)
{   // not used
    BaizeResetError(c->owner->owner);
    return false;
}

bool TableauPileTapped(struct Pile *p)
{
    BaizeResetError(p->owner);
    return false;
}

bool TableauCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    if ( PileEmpty(self) ) {
        return CheckAccept(baize, self, c);
    }
    return CheckPair(baize, PilePeekCard(self), c);
}

bool TableauCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    if ( PileEmpty(self) ) {
        return CheckAccept(baize, self, ArrayGet(tail, 0));
    }
    if ( ArrayLen(tail) == 1 ) {
        return CheckPair(baize, PilePeekCard(self), ArrayGet(tail, 0));
    } else {
        if ( baize->powerMoves ) {
            size_t moves = BaizePowerMoves(baize, self);
            if ( ArrayLen(tail) > moves ) {
                char z[128];
                sprintf(z, "Only enough space to move %lu cards, not %lu", moves, ArrayLen(tail));
                BaizeSetError(baize, z);
                return false;
            }
        }
        return CheckPair(baize, PilePeekCard(self), ArrayGet(tail, 0)) && CheckTail(baize, self, tail);
    }
}

int TableauCollect(struct Pile *const self)
{
    return PileGenericCollect(self);
}

bool TableauComplete(struct Pile *const self)
{
    (void)self;
    return false;
}

bool TableauConformant(struct Pile *const self)
{
    (void)self;
    return false;
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
    // iterate the pile->cards using an unsigned size_t, so make sure there are >2 before doing len-1
    if ( ArrayLen(self->cards) == 0 ) {
        return;
    }
    if ( ArrayLen(self->cards) == 1 ) {
        *sorted += 1;
        return;
    }
    // TODO this reeks of inefficiency
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
}

void TableauDraw(struct Pile *const self)
{
    extern Font fontAcme24 ;
    extern Color baizeHighlightColor;

    PileDraw(self);

    struct Tableau* t = (struct Tableau*)self;
    if ( t->accept != 0 ) {
        extern float cardWidth;
        float fontSize = cardWidth / 2.0f;
        Vector2 pos = PileScreenPos(self);
        pos.x += cardWidth / 8.0f;
        pos.y += cardWidth / 16.0f;
        DrawTextEx(fontAcme24, UtilOrdToShortString(t->accept), pos, fontSize, 0, baizeHighlightColor);
    }
}
