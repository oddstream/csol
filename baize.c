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

struct Baize* BaizeNew(const char* variantName) {

    char fname[64];
    int packs = 1;

    struct Baize* self = malloc(sizeof(struct Baize));
    self->magic = 0x12345678;

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

    self->piles = ArrayNew(32);

    // always create a stock pile
    struct Pile* stock = PileNew("Stock", (Vector2){0,0}, NONE);
    if ( stock ) {
        ArrayPush(&self->piles, (void**)stock);
        for ( int i=0; i<packs*52; i++ ) {
            PilePush(stock, &self->cardLibrary[i]);
        }
        self->stock = (struct Pile*)ArrayFirst(&self->piles, NULL);
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

    fprintf(stderr, "%d piles created\n", ArrayLen(&self->piles));

    self->stock = (struct Pile*)ArrayGet(&self->piles, 0);

    return self;
}

bool BaizeValid(struct Baize* self) {
    return self && self->magic == 0x12345678;
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
    self->magic = 0;
    free(self);
}
