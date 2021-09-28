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
    &TableauCanAcceptTail,
    &TableauCollect,
    &TableauComplete,
    &TableauConformant,
    &TableauSetAccept,
    &TableauSetRecycles,

    &PileUpdate,
    &TableauDraw,
    &PileFree,
};

struct Tableau* TableauNew(Vector2 slot, enum FanType fan, const char* buildfunc, const char* dragfunc)
{
    struct Tableau* self = malloc(sizeof(struct Tableau));
    if ( self ) {
        PileCtor((struct Pile*)self, "Tableau", slot, fan, buildfunc, dragfunc);
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

bool TableauCanAcceptTail(struct Baize *const baize, struct Pile *const self, struct Array *const tail)
{
    if ( ArrayLen(self->cards) == 0 ) {
        struct Tableau *t = (struct Tableau*)self;
        if ( t->accept != 0 ) {
            struct Card* c = ArrayGet(tail, 0);
            if ( c->id.ordinal != t->accept ) {
                char z[128];
                sprintf(z, "This empty tableau can only accept a %d, not a %d", t->accept, c->id.ordinal);
                BaizeSetError(self->owner, z);
                return false;
            }
        }
        return ConformantBuild(baize, self, tail);
    }
    return ConformantBuild(baize, self, tail);
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

void TableauDraw(struct Pile *const self)
{
    extern Font fontAcme24 ;
    extern Color baizeHighlightColor;

    PileDraw(self);

    struct Tableau* t = (struct Tableau*)self;
    if ( t->accept != 0 ) {
        Vector2 pos = PileScreenPos(self);
        pos.x += 10;
        pos.y += 10;
        DrawTextEx(fontAcme24, UtilOrdToShortString(t->accept), pos, 24, 0, baizeHighlightColor);
    }
}
