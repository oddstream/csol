/* baize.c */

#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "moon.h"
#include "baize.h"
#include "card.h"

#define MAGIC (0x79767)

struct Baize* BaizeNew(const char* variantName) {

    char fname[64];
    int packs = 1;

    struct Baize* self = malloc(sizeof(struct Baize));
    self->magic = MAGIC;

    self->L = luaL_newstate();
    luaL_openlibs(self->L);

    MoonRegisterFunctions(self->L);

    // create a handle to this Baize inside Lua
    lua_pushlightuserdata(self->L, self);
    lua_setglobal(self->L, "BAIZE");

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

    self->piles = ArrayNew(8);

    // always create a stock pile, and fill it
    struct Pile* stock = PileNew("Stock", (Vector2){0,0}, NONE);
    if ( stock ) {
        ArrayPush(self->piles, (void**)stock);
        for ( int i=0; i<packs*52; i++ ) {
            PilePush(stock, &self->cardLibrary[i]);
        }
        PileShuffle(stock);
        self->stock = (struct Pile*)ArrayFirst(self->piles);
    }

    fprintf(stderr, "stock now has %lu cards\n", PileLen(self->stock));

    int typ = lua_getglobal(self->L, "Build");  // push value of Build onto the stack
    if ( typ != LUA_TFUNCTION ) {
        fprintf(stderr, "Build is not a function\n");
    } else {
        if ( lua_pcall(self->L, 0, 0, 0) != LUA_OK ) {
            fprintf(stderr, "error running Lua function: %s\n", lua_tostring(self->L, -1));
            lua_pop(self->L, 1);
        } else {
            fprintf(stderr, "Build called ok\n");
        }
    }

    fprintf(stderr, "%d piles created\n", ArrayLen(self->piles));

    self->stock = (struct Pile*)ArrayGet(self->piles, 0);

    SetWindowTitle(variantName);

    return self;
}

bool BaizeValid(struct Baize *const self) {
    return self && self->magic == MAGIC;
}

static struct Card* findCardAt(struct Baize *const self, Vector2 pos) {
    struct Pile* p = (struct Pile*)ArrayFirst(self->piles);
    while ( p ) {
        struct Card* c = (struct Card*)ArrayLast(p->cards);
        while ( c ) {
            extern float cardWidth, cardHeight;
            Rectangle rect = {.x=c->baizePos.x, .y=c->baizePos.y, .width=cardWidth, .height=cardHeight};
            if ( CheckCollisionPointRec(pos, rect) ) {
                return c;
            }
            c = (struct Card*)ArrayPrev(p->cards);
        }
        p = (struct Pile*)ArrayNext(self->piles);
    }
    return NULL;
}

void BaizeUpdate(struct Baize *const self) {

    static struct Card* currCard = NULL;
    static float dx, dy;

    Vector2 touchPosition = GetTouchPosition(0);
    int gesture = GetGestureDetected();

    struct Card* ace = findCardAt(self, touchPosition);

    if ( gesture == GESTURE_TAP && ace ) {
        dx = touchPosition.x - ace->baizePos.x;
        dy = touchPosition.y - ace->baizePos.y;
        currCard = ace;
        CardStartDrag(currCard);
    }
    if ( gesture == GESTURE_DRAG && currCard ) {
        CardSetPosition(currCard, (Vector2){touchPosition.x - dx, touchPosition.y - dy});
    }
    if ( gesture == GESTURE_NONE ) {
        if ( currCard ) {
            // finished dragging
            CardStopDrag(currCard);
        }
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

    struct Pile* p = (struct Pile*)ArrayFirst(self->piles);
    while ( p ) {
        PileUpdate(p);
        p = (struct Pile*)ArrayNext(self->piles);
    }

}

void BaizeDraw(struct Baize *const self) {

    extern Color baizeColor;

    ClearBackground(baizeColor);
    BeginDrawing();

    struct Card* c;

    struct Pile* p = (struct Pile*)ArrayFirst(self->piles);
    while ( p ) {
        PileDraw(p);
        p = (struct Pile*)ArrayNext(self->piles);
    }

    p = (struct Pile*)ArrayFirst(self->piles);
    while ( p ) {
        c = (struct Card*)ArrayFirst(p->cards);
        while ( c ) {
            if ( !(CardTransitioning(c) || CardDragging(c)) ) {
                CardDraw(c);
            }
            c = (struct Card*)ArrayNext(p->cards);
        }
        p = (struct Pile*)ArrayNext(self->piles);
    }

    p = (struct Pile*)ArrayFirst(self->piles);
    while ( p ) {
        c = (struct Card*)ArrayFirst(p->cards);
        while ( c ) {
            if ( CardTransitioning(c) || CardDragging(c) ) {
                CardDraw(c);
            }
            c = (struct Card*)ArrayNext(p->cards);
        }
        p = (struct Pile*)ArrayNext(self->piles);
    }

    DrawFPS(10, 10);
    EndDrawing();
}

void BaizeFree(struct Baize *const self) {
    ArrayForeach(self->piles, (ArrayIterFunc)PileFree);
    ArrayFree(self->piles);
    free(self->cardLibrary);
    lua_close(self->L);
    self->magic = 0;
    free(self);
}
