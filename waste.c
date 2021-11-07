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
    &PileInertPileTapped,
    &PileGenericTailTapped,
    &WasteCollect,
    &WasteComplete,
    &WasteConformant,
    &PileInertSetRecycles,
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

_Bool WasteCanMoveTail(struct Array *const tail)
{
    if (ArrayLen(tail)>1) {
        struct Card *c = ArrayGet(tail, 0);
        struct Baize *baize = PileOwner(CardOwner(c));
        BaizeSetError(baize, "(CSOL) Only a single card can be moved from Waste");
        return 0;
    }
    return 1;
}

_Bool WasteCanAcceptCard(struct Baize *const baize, struct Pile *const self, struct Card *const c)
{
    (void)baize;

    struct Array1 tail = Array1New(c);
    return CanTailBeAppended(self, (struct Array*)&tail);
    // don't need to free an Array1
}

_Bool WasteCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
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

_Bool WasteComplete(struct Pile *const self)
{
    return PileEmpty(self);
}

_Bool WasteConformant(struct Pile *const self)
{
    return PileEmpty(self);
}

void WasteCountSortedAndUnsorted(struct Pile *const self, int *sorted, int *unsorted)
{
    (void)sorted;
    *unsorted += ArrayLen(self->cards);
}
