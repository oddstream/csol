/* baize.c */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "moon.h"
#include "baize.h"
#include "stock.h"
#include "card.h"
#include "util.h"

#define MAGIC (0x19910920)

struct Baize* BaizeNew(const char* variantName) {

    char fname[64];
    int packs = 1;

    struct Baize* self = malloc(sizeof(struct Baize));
    self->magic = MAGIC;

    self->L = luaL_newstate();
    luaL_openlibs(self->L);

    MoonRegisterFunctions(self->L);

    // create a handle to this Baize inside Lua TODO maybe not needed
    lua_pushlightuserdata(self->L, self);   lua_setglobal(self->L, "BAIZE");

    lua_pushinteger(self->L, NONE);         lua_setglobal(self->L, "FAN_NONE");
    lua_pushinteger(self->L, DOWN);         lua_setglobal(self->L, "FAN_DOWN");
    lua_pushinteger(self->L, LEFT);         lua_setglobal(self->L, "FAN_LEFT");
    lua_pushinteger(self->L, RIGHT);        lua_setglobal(self->L, "FAN_RIGHT");
    lua_pushinteger(self->L, WASTE_DOWN);   lua_setglobal(self->L, "FAN_WASTEDOWN");
    lua_pushinteger(self->L, WASTE_LEFT);   lua_setglobal(self->L, "FAN_WASTELEFT");
    lua_pushinteger(self->L, WASTE_RIGHT);  lua_setglobal(self->L, "FAN_WASTERIGHT");

    sprintf(fname, "variants/%s.lua", variantName);

    if ( luaL_loadfile(self->L, fname) || lua_pcall(self->L, 0, 0, 0) ) {
        fprintf(stderr, "%s\n", lua_tostring(self->L, -1));
        lua_pop(self->L, 1);
    } else {
        // TODO read baizeColor &c
        packs = MoonGetGlobalInt(self->L, "PACKS", 1);
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
    self->stock = (struct Pile*)StockNew((Vector2){0,0}, NONE, NULL, NULL);
    lua_pushlightuserdata(self->L, self->stock);   lua_setglobal(self->L, "STOCK");
    if ( PileValid(self->stock) ) {
        self->stock->owner = self;
        ArrayPush(self->piles, self->stock);
        for ( int i=0; i<packs*52; i++ ) {
            PilePushCard(self->stock, &self->cardLibrary[i]);
        }
        // Knuth-Fisher–Yates shuffle
        srand(time(NULL));
        size_t n = ArrayLen(self->stock->cards);
        for ( int i = n-1; i > 0; i-- ) {
            int j = rand() % (i+1);
            ArraySwap(self->stock->cards, i, j);
        }
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
    self->touchedPile = NULL;

    SetWindowTitle(variantName);

    return self;
}

bool BaizeValid(struct Baize *const self)
{
    return self && self->magic == MAGIC;
}

struct Pile* BaizeFindPile(struct Baize* self, const char* category, int n)
{
    size_t index;
    struct Pile* p = (struct Pile*)ArrayFirst(self->piles, &index);
    while ( p ) {
        if ( strcmp(p->category, category) == 0 ) {
            n--;
            if ( n == 0 ) {
                return p;
            }
        }
        p = (struct Pile*)ArrayNext(self->piles, &index);
    }
    return NULL;
}

static struct Card* findCardAt(struct Baize *const self, Vector2 pos)
{
    size_t pindex, cindex;
    struct Pile* p = (struct Pile*)ArrayFirst(self->piles, &pindex);
    while ( p ) {
        struct Card* c = (struct Card*)ArrayLast(p->cards, &cindex);
        while ( c ) {
            if ( CardIsAt(c, pos) ) {
                return c;
            }
            // extern float cardWidth, cardHeight;
            // Rectangle rect = {.x=c->pos.x, .y=c->pos.y, .width=cardWidth, .height=cardHeight};
            // if ( CheckCollisionPointRec(pos, rect) ) {
            //     return c;
            // }
            c = (struct Card*)ArrayPrev(p->cards, &cindex);
        }
        p = (struct Pile*)ArrayNext(self->piles, &pindex);
    }
    return NULL;
}

static struct Pile* findPileAt(struct Baize *const self, Vector2 pos)
{
    size_t pindex;
    struct Pile* p = (struct Pile*)ArrayFirst(self->piles, &pindex);
    while ( p ) {
        if ( PileIsAt(p, pos) ) {
            return p;
        }
        p = (struct Pile*)ArrayNext(self->piles, &pindex);
    }
    return NULL;
}

static struct Pile* largestIntersection(struct Baize *const self, struct Card *const c)
{
    float largestArea = 0.0;
    struct Pile *pile = NULL;
    Rectangle rectCard = CardGetRect(c);
    size_t index;
    for ( struct Pile *p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        if ( p == CardGetOwner(c) ) {
            continue;
        }
        Rectangle rectPile = PileGetFannedRect(p);
        float area = UtilOverlapArea(rectCard, rectPile);
        if ( area > largestArea ) {
            largestArea = area;
            pile = p;
        }
    }
    return pile;
}

void BaizeMakeTail(struct Baize *const self, struct Card *const cFirst)
{
    if ( self->tail ) {
        ArrayFree(self->tail);
        self->tail = NULL;
    }
    size_t index = 0;
    struct Pile* p = CardGetOwner(cFirst);
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

void BaizeTouchStart(struct Baize *const self, Vector2 touchPosition)
{
    struct Card* c = findCardAt(self, touchPosition);
    if ( c ) {
        // record the distance from the card's origin to the tap point
        // dx = touchPosition.x - c->pos.x;
        // dy = touchPosition.y - c->pos.y;

        BaizeMakeTail(self, c);
        if ( self->tail ) {
            ArrayForeach(self->tail, (ArrayIterFunc)CardStartDrag);
        }
        self->lastTouch = touchPosition;
    } else {
        self->touchedPile = findPileAt(self, touchPosition);    // could be NULL
    }
}

void BaizeTouchMove(struct Baize *const self, Vector2 touchPosition)
{
    if ( self->tail ) {
        size_t index;
        struct Card* c = (struct Card*)ArrayFirst(self->tail, &index);
        while ( c ) {
            Vector2 delta = {.x = touchPosition.x - self->lastTouch.x, .y = touchPosition.y - self->lastTouch.y};
            CardMovePositionBy(c, delta);
            c = (struct Card*)ArrayNext(self->tail, &index);
        }
    } else if ( self->touchedPile ) {
        // do nothing, can't drag a pile
    } else {
        // TODO drag the baize
    }
    self->lastTouch = touchPosition;
}

void BaizeTouchStop(struct Baize *const self)
{
    if ( self->tail ) {
        size_t index;
        struct Card* c = (struct Card*)ArrayFirst(self->tail, &index);
        struct Card *cHeadOfTail = c;
        if ( CardWasDragged(c) ) {
            struct Pile* p = largestIntersection(self, c);
            if ( p ) {
                // fprintf(stderr, "Intersection with %s\n", p->category);
                if ( p->vtable->CanAcceptTail(p, self->L, self->tail) ) {
                    while ( c ) {
                        CardStopDrag(c);
                        c = (struct Card*)ArrayNext(self->tail, &index);
                    }
                    PileMoveCards(p, cHeadOfTail);
                } else {
                    // fprintf(stderr, "cannot move cards there\n");
                    while ( c ) {
                        CardCancelDrag(c);
                        c = (struct Card*)ArrayNext(self->tail, &index);
                    }
                }
            } else {
                // fprintf(stderr, "No intersection\n");
                while ( c ) {
                    CardCancelDrag(c);
                    c = (struct Card*)ArrayNext(self->tail, &index);
                }
            }
        } else {
            // char z[4];
            // CardShorthand(c, z);
            // fprintf(stdout, "Card %s tapped\n", z);
            while ( c ) {
                CardStopDrag(c);    // CardCancelDrag() would use CardTransitionTo(), and we know the card didn't move
                c = (struct Card*)ArrayNext(self->tail, &index);
            }
            cHeadOfTail->owner->vtable->CardTapped(self->L, cHeadOfTail);
        }
        ArrayFree(self->tail);
        self->tail = NULL;
    } else if ( self->touchedPile ) {
        self->touchedPile->vtable->PileTapped(self->L, self->touchedPile);
        self->touchedPile = NULL;
    } else {
        // TODO finish dragging baize
    }
}

void BaizeUpdate(struct Baize *const self)
{
    // static float dx, dy;

    Vector2 touchPosition = GetTouchPosition(0);
    int gesture = GetGestureDetected();

    if ( gesture == GESTURE_TAP && !self->tail ) {
        BaizeTouchStart(self, touchPosition);
    }
    if ( gesture == GESTURE_DRAG && (self->tail || self->touchedPile) ) {
        BaizeTouchMove(self, touchPosition);
    }
    if ( gesture == GESTURE_NONE && (self->tail || self->touchedPile) ) {
        BaizeTouchStop(self);
    }

    // int dx = c.rect.x - touchPosition.x;
    // int dy = c.rect.y - touchPosition.y;
    // {
    //     char buf[64];
    //     sprintf(buf, "dragging %d,%d", dx, dy);
    //     DrawText(buf, 0, 130, 16, WHITE);
    // }
    // CardSetPos((struct Card*)ArrayGet(self->tail, 0), (Vector2){touchPosition.x - dx, touchPosition.y - dy});

    ArrayForeach(self->piles, (ArrayIterFunc)PileUpdate);

}

void BaizeDraw(struct Baize *const self)
{
    extern Color baizeColor;

    ClearBackground(baizeColor);
    BeginDrawing();

    struct Card* c;
    size_t pindex, cindex;
    struct Pile* p = (struct Pile*)ArrayFirst(self->piles, &pindex);
    while ( p ) {
        // PileDraw(p);
        p->vtable->Draw(p);
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

    // {
    //     Vector2 touchPosition = GetTouchPosition(0);
    //     c = findCardAt(self, touchPosition);
    //     if ( c ) {
    //         CardDrawRect(c, 10, 100);
    //     }
    // }
    // DrawFPS(10, 10);
    EndDrawing();
}

void BaizeFree(struct Baize *const self)
{
    ArrayFree(self->tail);
    ArrayForeach(self->piles, (ArrayIterFunc)PileFree);
    ArrayFree(self->piles);
    free(self->cardLibrary);
    lua_close(self->L);
    self->magic = 0;
    free(self);
}
