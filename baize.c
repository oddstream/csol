/* baize.c */

#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>

#include "moon.h"
#include "baize.h"
#include "card.h"

struct Baize* BaizeNew(const char* variantName) {

    char fname[64];
    int packs = 1;

    struct Baize* self = malloc(sizeof(struct Baize));

    self->L = luaL_newstate();

    sprintf(fname, "variants/%s.lua", variantName);

    if ( luaL_loadfile(self->L, fname) || lua_pcall(self->L, 0, 0, 0) ) {
        fprintf(stderr, "%s\n", lua_tostring(self->L, -1));
        lua_pop(self->L, 1);
    } else {
        packs = MoonGetGlobalInt(self->L, "Packs", 1);
    }

    self->cardLibrary = calloc(packs * 52, sizeof(struct Card));
    for ( int pack = 0; pack < packs; pack++ ) {
        int i = 0;
        for ( enum CardSuit s = CLUB; s<=SPADE; s++ ) {
            for ( enum CardOrdinal o = ACE; o <= KING; o++ ) {
                self->cardLibrary[i++] = CardNew(s, o);
            }
        }
    }

    self->piles = ArrayNew(16);

    // call BuildVariant() in variant.lua

    self->stock = PileNew("Stock", (Vector2){100, 100}, NONE);
    ArrayPush(&self->piles, (void**)self->stock);
    PilePush(self->stock, &self->cardLibrary[0]);

    struct Pile* pf = PileNew("Foundation", (Vector2){200, 100}, DOWN);
    ArrayPush(&self->piles, (void**)pf);
    PilePush(pf, &self->cardLibrary[1]);

    pf = PileNew("Foundation", (Vector2){300, 100}, DOWN);
    ArrayPush(&self->piles, (void**)pf);
    PilePush(pf, &self->cardLibrary[2]);

    pf = PileNew("Foundation", (Vector2){400, 100}, DOWN);
    ArrayPush(&self->piles, (void**)pf);
    PilePush(pf, &self->cardLibrary[3]);

    pf = PileNew("Foundation", (Vector2){500, 100}, DOWN);
    ArrayPush(&self->piles, (void**)pf);
    PilePush(pf, &self->cardLibrary[4]);

    return self;
}

void BaizeUpdate(struct Baize* self) {

    (void)self;

    static struct Card* currCard = NULL;
    static float dx, dy;

    Vector2 touchPosition = GetTouchPosition(0);
    int gesture = GetGestureDetected();

    // TODO search piles from bottom to top, not this...
    struct Card* ace = NULL;
    for ( int i=0; i<52; i++ ) {
        struct Card* c = &self->cardLibrary[i];
        if ( CheckCollisionPointRec(touchPosition, c->rect) ) {
            ace = c;
            break;
        }
    }

    // if ( gesture == GESTURE_TAP && CardIsAt(ace, touchPosition) ) {
    if ( gesture == GESTURE_TAP && ace ) {
        dx = touchPosition.x - ace->rect.x;
        dy = touchPosition.y - ace->rect.y;
        currCard = ace;
    }
    if ( gesture == GESTURE_DRAG && currCard ) {
        // card_position(&c, touchPosition.x - c.rect.width / 2, touchPosition.y - c.rect.height / 2);
        CardSetPosition(currCard, (Vector2){touchPosition.x - dx, touchPosition.y - dy});
    }
    if ( gesture == GESTURE_NONE ) {
        currCard = NULL;
    }

    // if ( pc == NULL ) {
    //     if ( card_isAt(&c, touchPosition) ) {
    //         DrawText("touch on card", 0, 100, 16, WHITE);
    //         pc = &c;
    //     }
    // }

    // int dx = c.rect.x - touchPosition.x;
    // int dy = c.rect.y - touchPosition.y;
    // {
    //     char buf[64];
    //     sprintf(buf, "dragging %d,%d", dx, dy);
    //     DrawText(buf, 0, 130, 16, WHITE);
    // }
    // card_position(&c, touchPosition.x - dx, touchPosition.y - dy);
}

void BaizeDraw(struct Baize* self) {
    // struct Card* ace = PilePeek(b->stock);

    extern Color baizeColor;

    ClearBackground(baizeColor);
    BeginDrawing();
    int i = 0;
    struct Pile* p = (struct Pile*)ArrayFirst(&self->piles, &i);
    while ( p ) {
        PileDraw(p);
        int j = 0;
        struct Card* c = (struct Card*)ArrayFirst(&p->cards, &j);
        while ( c ) {
            CardDraw(c);
            c = (struct Card*)ArrayNext(&p->cards, &j);
        }
        p = (struct Pile*)ArrayNext(&self->piles, &i);
    }
    DrawFPS(10, 10);
    EndDrawing();
}

void BaizeFree(struct Baize* self) {
    // for ( int i = 0; i<ArrayLen(&self->piles); i++ ) {
    //     struct Pile* pp = (struct Pile*)ArrayGet(&self->piles, i);
    //     PileFree(pp);
    // }
    ArrayForeach(&self->piles, (ArrayIterFunc)PileFree);
    ArrayFree(&self->piles);
    free(self->cardLibrary);
    lua_close(self->L);
    free(self);
}
