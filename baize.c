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
        for ( enum CardOrdinal o = ACE; o <= KING; o++ ) {
            for ( enum CardSuit s = CLUB; s <= SPADE; s++ ) {
                self->cardLibrary[i++] = CardNew(o, s);
            }
        }
    }

    self->piles = ArrayNew(8);

    // always create a stock pile, and fill it
    self->stock = PileNew("Stock", (Vector2){0,0}, NONE);
    if ( self->stock ) {
        ArrayPush(self->piles, (void**)self->stock);
        for ( int i=0; i<packs*52; i++ ) {
            PilePush(self->stock, &self->cardLibrary[i]);
        }
        PileShuffle(self->stock);
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

    fprintf(stderr, "%lu piles created\n", ArrayLen(self->piles));

    self->tail = NULL;

    SetWindowTitle(variantName);

    return self;
}

bool BaizeValid(struct Baize *const self) {
    return self && self->magic == MAGIC;
}

static struct Card* findCardAt(struct Baize *const self, Vector2 pos) {
    size_t pindex, cindex;
    struct Pile* p = (struct Pile*)ArrayFirst(self->piles, &pindex);
    while ( p ) {
        struct Card* c = (struct Card*)ArrayLast(p->cards, &cindex);
        while ( c ) {
            extern float cardWidth, cardHeight;
            Rectangle rect = {.x=c->baizePos.x, .y=c->baizePos.y, .width=cardWidth, .height=cardHeight};
            if ( CheckCollisionPointRec(pos, rect) ) {
                return c;
            }
            c = (struct Card*)ArrayPrev(p->cards, &cindex);
        }
        p = (struct Pile*)ArrayNext(self->piles, &pindex);
    }
    return NULL;
}

void BaizeMakeTail(struct Baize *const self, struct Card *const cFirst) {
    
    if ( self->tail ) {
        ArrayFree(self->tail);
        self->tail = NULL;
    }
    size_t index = 0;
    struct Pile* p = cFirst->owner;
    struct Card* c = (struct Card*)ArrayFirst(p->cards, &index);
    while ( c ) {
        if ( c == cFirst ) {
            self->tail = ArrayNew(ArrayCap(p->cards));
            ArrayCopyTail(self->tail, p->cards, index);
            break;
        }
        c = (struct Card*)ArrayNext(p->cards, &index);
    }
}

void BaizeUpdate(struct Baize *const self) {

    // static float dx, dy;

    Vector2 touchPosition = GetTouchPosition(0);
    static Vector2 lastTouch = {0};
    int gesture = GetGestureDetected();

    if ( gesture == GESTURE_TAP && !self->tail ) {
        struct Card* ace = findCardAt(self, touchPosition);
        if ( ace ) {
            // record the distance from the card's origin to the tap point
            // dx = touchPosition.x - ace->baizePos.x;
            // dy = touchPosition.y - ace->baizePos.y;

            BaizeMakeTail(self, ace);
            if ( self->tail ) {
                ArrayForeach(self->tail, (ArrayIterFunc)CardStartDrag);
            }
            lastTouch = touchPosition;
        }
    }
    if ( gesture == GESTURE_DRAG && self->tail ) {
        // CardSetPosition((struct Card*)ArrayGet(self->tail, 0), (Vector2){touchPosition.x - dx, touchPosition.y - dy});
        size_t index;
        struct Card* c = (struct Card*)ArrayFirst(self->tail, &index);
        while ( c ) {
            Vector2 delta = {.x = touchPosition.x - lastTouch.x, .y = touchPosition.y - lastTouch.y};
            CardMovePositionBy(c, delta);
            c = (struct Card*)ArrayNext(self->tail, &index);
        }
        lastTouch = touchPosition;
    }
    if ( gesture == GESTURE_NONE && self->tail ) {
        // finished dragging
        size_t index;
        struct Card* c = (struct Card*)ArrayFirst(self->tail, &index);
        if ( CardNotDragged(c) ) {
            char z[4];
            CardShorthand(c, z);
            fprintf(stderr, "Card %s tapped\n", z);
        }
        while ( c ) {
            CardStopDrag(c);
            c = (struct Card*)ArrayNext(self->tail, &index);
        }
        ArrayFree(self->tail);
        self->tail = NULL;
    }

    // int dx = c.rect.x - touchPosition.x;
    // int dy = c.rect.y - touchPosition.y;
    // {
    //     char buf[64];
    //     sprintf(buf, "dragging %d,%d", dx, dy);
    //     DrawText(buf, 0, 130, 16, WHITE);
    // }

    ArrayForeach(self->piles, (ArrayIterFunc)PileUpdate);

}

void BaizeDraw(struct Baize *const self) {

    extern Color baizeColor;

    ClearBackground(baizeColor);
    BeginDrawing();

    struct Card* c;
    size_t pindex, cindex;
    struct Pile* p = (struct Pile*)ArrayFirst(self->piles, &pindex);
    while ( p ) {
        PileDraw(p);
        c = (struct Card*)ArrayFirst(p->cards, &cindex);
        while ( c ) {
            if ( !(CardTransitioning(c) || CardDragging(c)) ) {
                CardDraw(c);
            }
            c = (struct Card*)ArrayNext(p->cards, &cindex);
        }
        p = (struct Pile*)ArrayNext(self->piles, &pindex);
    }

    p = (struct Pile*)ArrayFirst(self->piles, &pindex);
    while ( p ) {
        c = (struct Card*)ArrayFirst(p->cards, &cindex);
        while ( c ) {
            if ( CardTransitioning(c) || CardDragging(c) ) {
                CardDraw(c);
            }
            c = (struct Card*)ArrayNext(p->cards, &cindex);
        }
        p = (struct Pile*)ArrayNext(self->piles, &pindex);
    }

    DrawFPS(10, 10);
    EndDrawing();
}

void BaizeFree(struct Baize *const self) {
    ArrayFree(self->tail);
    ArrayForeach(self->piles, (ArrayIterFunc)PileFree);
    ArrayFree(self->piles);
    free(self->cardLibrary);
    lua_close(self->L);
    self->magic = 0;
    free(self);
}
