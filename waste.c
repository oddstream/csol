/* waste.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>

#include "baize.h"
#include "pile.h"
#include "array.h"
#include "constraint.h"
#include "waste.h"

static struct PileVtable wasteVtable = {
    &WasteCanMoveTail,
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

struct Waste* WasteNew(struct Baize *const baize, Vector2 slot, enum FanType fan)
{
    struct Waste* self = calloc(1, sizeof(struct Waste));
    if ( self ) {
        PileCtor(baize, (struct Pile*)self, "Waste", slot, fan);
        self->super.vtable = &wasteVtable;
    }
    return self;
}

bool WasteCanMoveTail(struct Array *const tail)
{
    if (ArrayLen(tail)>1) {
        struct Card *c = ArrayGet(tail, 0);
        struct Baize *baize = CardToBaize(c);
        BaizeSetError(baize, "(C) Only a single card can be moved from Waste");
        return false;
    }
    return true;
}

bool WasteCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    (void)baize;

    struct Array1 tail =(struct Array1){.size=1, .used=1, .data[0]=c};
    return CanTailBeAppended(self, (struct Array*)&tail);
    // don't need to free an Array1
}

bool WasteCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    if (ArrayLen(tail) == 1) {
        return WasteCanAcceptCard(baize, self, ArrayGet(tail, 0));
    }
    return CanTailBeAppended(self, tail);
}

int WasteCollect(struct Pile *const self)
{
    return PileGenericCollect(self);
}

bool WasteComplete(struct Pile *const self)
{
    return PileEmpty(self);
}

bool WasteConformant(struct Pile *const self)
{
    return PileEmpty(self);
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
