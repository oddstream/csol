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
        return ConformantBuild(baize, self, NULL, tail);
    }
    return ConformantBuild(baize, self, PilePeekCard(self), tail);
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
    PileDraw(self);

    struct Tableau* t = (struct Tableau*)self;
    if ( t->accept != 0 ) {
        // extern Font fontAcme;
        Vector2 pos = PileGetScreenPos(self);
        pos.x += 10;
        pos.y += 10;
        // DrawTextEx(fontAcme, ords[f->accept], pos, 16, 0, (Color){255,255,255,127});
        DrawText(UtilOrdToShortString(t->accept), (int)pos.x, (int)pos.y, 32, (Color){255,255,255,31});
    }
}