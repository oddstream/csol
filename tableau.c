/* tableau.c */

#include <stdlib.h>
#include <raylib.h>
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

struct Tableau* TableauNew(Vector2 pos, enum FanType fan, const char* buildfunc, const char* dragfunc)
{
    struct Tableau* self = malloc(sizeof(struct Tableau));
    if ( self ) {
        PileCtor((struct Pile*)self, "Tableau", pos, fan, buildfunc, dragfunc);
        self->super.vtable = &tableauVtable;
        self->accept = 0;   // accept any by default
    }
    return self;
}

void TableauCardTapped(struct Card *c)
{
    (void)c;
}

void TableauPileTapped(struct Pile *p)
{
    (void)p;
}

bool TableauCanAcceptTail(struct Pile *const self, lua_State *L, struct Array *const tail)
{
    if ( ArrayLen(self->cards) == 0 ) {
        struct Tableau *t = (struct Tableau*)self;
        if ( t->accept != 0 ) {
            struct Card* c = ArrayGet(tail, 0);
            if ( c->id.ordinal != t->accept ) {
                fprintf(stderr, "The empty tableau can only accept a %d, not a %d\n", t->accept, c->id.ordinal);
                return false;
            }
        }
        return ConformantBuildTail(L, self, tail);
    }
    return ConformantBuildAppend(L, self, tail);
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
        Vector2 pos = PileGetPos(self);
        pos.x += 10;
        pos.y += 10;
        // DrawTextEx(fontAcme, ords[f->accept], pos, 16, 0, (Color){255,255,255,127});
        DrawText(UtilOrdToShortString(t->accept), (int)pos.x, (int)pos.y, 24, (Color){255,255,255,31});
    }
}
