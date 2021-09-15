/* foundation.c */

#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <lua.h>

#include "pile.h"
#include "array.h"
#include "foundation.h"
#include "conformant.h"
#include "util.h"

static struct PileVtable foundationVtable = {
    &PileValid,
    &PileLen,

    &PilePushCard,
    &PilePopCard,
    &PilePeekCard,
    &PileMoveCards,

    &PileGetRect,
    &PileGetPos,
    &PileSetPos,
    &PileGetFannedRect,
    &PileGetPushedFannedPos,

    &FoundationCanAcceptTail,
    &FoundationSetAccept,

    &PileUpdate,
    &FoundationDraw,
    &PileFree,
};

struct Foundation* FoundationNew(Vector2 pos, enum FanType fan, const char* buildfunc, const char* dragfunc) {
    struct Foundation* self = malloc(sizeof(struct Foundation));
    if ( self ) {
        PileCtor((struct Pile*)self, "Foundation", pos, fan, buildfunc, dragfunc);
        self->super.vtable = &foundationVtable;
        self->accept = 0; // accept any by default
    }
    return self;
}

bool FoundationCanAcceptTail(struct Pile *const self, lua_State *L, struct Array *const tail) {
    if ( ArrayLen(tail) != 1 ) {
        fprintf(stderr, "Can only move single cards to a foundation\n");
        return false;
    }
    if ( ArrayLen(self->cards) == 13 ) {
        fprintf(stderr, "The foundation is full\n");
        return false;
    }
    if ( ArrayLen(self->cards) == 0 ) {
        struct Foundation *f = (struct Foundation*)self;
        if ( f->accept != 0 ) {
            struct Card* c = ArrayPeek(tail);
            if ( c->ord != f->accept ) {
                fprintf(stderr, "The foundation can only accept a %d, not a %d\n", f->accept, c->ord);
                return false;
            }
        }
        return true;
    }
    return ConformantBuildAppend(L, self, tail);
}

void FoundationSetAccept(struct Pile *const self, enum CardOrdinal ord) {
    ((struct Foundation*)self)->accept = ord;
}

void FoundationDraw(struct Pile *const self) {
    PileDraw(self);

    struct Foundation* f = (struct Foundation*)self;
    if ( f->accept != 0 ) {
        // extern Font fontAcme;
        Vector2 pos = PileGetPos(self);
        pos.x += 10;
        pos.y += 10;
        // DrawTextEx(fontAcme, ords[f->accept], pos, 16, 0, (Color){255,255,255,127});
        DrawText(UtilOrdToShortString(f->accept), (int)pos.x, (int)pos.y, 24, (Color){255,255,255,31});
    }
}
