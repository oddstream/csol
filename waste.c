/* waste.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "waste.h"

static struct PileVtable wasteVtable = {
    &WasteCanAcceptCard,
    &WasteCanAcceptTail,
    &WasteCollect,
    &WasteComplete,
    &WasteConformant,
    &WasteSetAccept,
    &WasteSetRecycles,
    &WasteCountSortedAndUnsorted,

    &PileUpdate,
    &PileDraw,
    &PileFree,
};

struct Waste* WasteNew(Vector2 slot, enum FanType fan)
{
    struct Waste* self = calloc(1, sizeof(struct Waste));
    if ( self ) {
        PileCtor((struct Pile*)self, "Waste", slot, fan);
        self->super.vtable = &wasteVtable;
    }
    return self;
}

bool WasteCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    (void)self;

    BaizeResetError(baize);

    if ( c->owner == baize->stock ) {
        return true;
    } else {
        BaizeSetError(baize, "You can only move cards to a Waste pile from the Stock");
    }
    return false;
}

bool WasteCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    (void)self;

    BaizeResetError(baize);
    // TODO maybe move three cards
    if ( ArrayLen(tail) == 1 ) {
        struct Card *c = ArrayGet(tail, 0);
        if ( c && c->owner == baize->stock ) {
            return true;
        } else {
            BaizeSetError(baize, "You can only move cards to a Waste pile from the Stock");
        }
    } else {
        BaizeSetError(baize, "You can only move one card to a Waste pile");
    }
    return false;
}

int WasteCollect(struct Pile *const self)
{
    return PileGenericCollect(self);
}

bool WasteComplete(struct Pile *const self)
{
    (void)self;
    return false;
}

bool WasteConformant(struct Pile *const self)
{
    (void)self;
    return false;
}

void WasteSetAccept(struct Pile *const self, enum CardOrdinal ord)
{
    // we don't do that here
    (void)self;
    (void)ord;
}

void WasteSetRecycles(struct Pile *const self, int r)
{
    // we don't do that here
    (void)self;
    (void)r;
}

void WasteCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
    (void)sorted;
    *unsorted += ArrayLen(self->cards);
}
