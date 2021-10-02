/* baize.c */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "csol.h"

#define BAIZE_MAGIC (0x19910920)

struct Baize* BaizeNew() {

    // extern char variantName[64];

    // fprintf(stdout, "CardID is %lu bytes\n", sizeof(struct CardId));    // 4
    // fprintf(stdout, "Card is %lu bytes\n", sizeof(struct Card));    // 72 (2021.09.17)
    // fprintf(stdout, "unsigned is %lu bytes\n", sizeof(unsigned));   4
    // fprintf(stdout, "unsigned long is %lu bytes\n", sizeof(unsigned long)); // 8
    // fprintf(stdout, "void* is %lu bytes\n", sizeof(void*)); // 8

    // char fname[128];
    // unsigned packs = 1;

    struct Baize* self = calloc(1, sizeof(struct Baize));
    if ( !self ) {
        return NULL;
    }
    self->magic = BAIZE_MAGIC;

    self->L = luaL_newstate();
    luaL_openlibs(self->L);

    MoonRegisterFunctions(self->L);

    // create a handle to this Baize inside Lua TODO maybe not needed
    lua_pushlightuserdata(self->L, self);   lua_setglobal(self->L, "BAIZE");

    lua_pushinteger(self->L, FAN_NONE);    lua_setglobal(self->L, "FAN_NONE");
    lua_pushinteger(self->L, FAN_DOWN);    lua_setglobal(self->L, "FAN_DOWN");
    lua_pushinteger(self->L, FAN_LEFT);    lua_setglobal(self->L, "FAN_LEFT");
    lua_pushinteger(self->L, FAN_RIGHT);   lua_setglobal(self->L, "FAN_RIGHT");
    lua_pushinteger(self->L, FAN_DOWN3);   lua_setglobal(self->L, "FAN_DOWN3");
    lua_pushinteger(self->L, FAN_LEFT3);   lua_setglobal(self->L, "FAN_LEFT3");
    lua_pushinteger(self->L, FAN_RIGHT3);  lua_setglobal(self->L, "FAN_RIGHT3");

    self->ui = UiNew();

    return self;
}

bool BaizeValid(struct Baize *const self)
{
    return self && self->magic == BAIZE_MAGIC;
}

void BaizeSetError(struct Baize *const self, const char *str)
{
    BaizeResetError(self);
    self->errorString = strdup(str);
}

void BaizeResetError(struct Baize *const self)
{
    if ( self->errorString ) {
        free(self->errorString);
        self->errorString = NULL;
    }
}

void BaizeCreateCards(struct Baize *const self)
{
    // the Baize object has been created by BaizeNew
    // or has been used to play another variant
    // reset/create piles Array
    // open variant.lua, get PACKS, create cardLibrary

    // TODO record lost game if current game started

    // Lua global PACKS will still be set from first run of variant.lua

    { // scope for fname
        extern char variantName[64];

        char fname[128];

        snprintf(fname, 127, "variants/%s.lua", variantName);

        if ( luaL_loadfile(self->L, fname) || lua_pcall(self->L, 0, 0, 0) ) {
            fprintf(stderr, "%s\n", lua_tostring(self->L, -1));
            lua_pop(self->L, 1);
            return;
        }

        UiUpdateTitleBar(self->ui, variantName);
    }

    {   // scope for packs, suits, i
        size_t packs = MoonGetGlobalInt(self->L, "PACKS", 1);
        size_t suits = MoonGetGlobalInt(self->L, "SUITS", 4);
        self->cardsInLibrary = packs * suits * 13;
        self->cardLibrary = calloc(self->cardsInLibrary, sizeof(struct Card));
        size_t i = 0;
        for ( size_t pack = 0; pack < packs; pack++ ) {
            for ( enum CardOrdinal o = ACE; o <= KING; o++ ) {
                for ( enum CardSuit s = 0; s < suits; s++ ) {
                    self->cardLibrary[i++] = CardNew(pack, o, s);
                }
            }
        }
    }

    self->powerMoves = MoonGetGlobalBool(self->L, "POWERMOVES", false);
}

void BaizeCreatePiles(struct Baize *const self)
{
    // reset the old piles
    if ( self->piles ) {
        size_t pindex;
        for ( struct Pile* p = ArrayFirst(self->piles, &pindex); p; p = ArrayNext(self->piles, &pindex) ) {
            PileFree(p);
        }
        ArrayReset(self->piles);
    } else {
        self->piles = ArrayNew(8);
    }

    self->stock = (struct Pile*)StockNew((Vector2){0,0}, FAN_NONE);
    if ( PileValid(self->stock) ) {
        lua_pushlightuserdata(self->L, self->stock);   lua_setglobal(self->L, "STOCK");
        self->stock->owner = self;
        ArrayPush(self->piles, self->stock);
        for ( size_t i = 0; i < self->cardsInLibrary; i++ ) {
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

    fprintf(stderr, "stock has %lu cards\n", PileLen(self->stock));

    int typ = lua_getglobal(self->L, "Build");  // push value of Build onto the stack
    if ( typ != LUA_TFUNCTION ) {
        fprintf(stderr, "Build is not a function\n");
        lua_pop(self->L, 1);    // remove "Build" from stack
    } else {
        if ( lua_pcall(self->L, 0, 0, 0) != LUA_OK ) {
            fprintf(stderr, "error running Lua function: %s\n", lua_tostring(self->L, -1));
            lua_pop(self->L, 1);
        } else {
            fprintf(stderr, "Build called ok\n");
        }
    }

    fprintf(stderr, "%lu piles created\n", ArrayLen(self->piles));

    // create handy shortcuts for waste, foundations and tableau
    {
        self->waste = NULL;
        if ( self->foundations ) {
            ArrayReset(self->foundations);
        } else {
            self->foundations = ArrayNew(8);
        }
        if ( self->tableaux ) {
            ArrayReset(self->tableaux);
        } else {
            self->tableaux = ArrayNew(8);
        }
        size_t pindex;
        for ( struct Pile *p = ArrayFirst(self->piles, &pindex); p; p = ArrayNext(self->piles, &pindex) ) {
            if ( strcmp(p->category, "Waste") == 0 ) {
                self->waste = p;
                lua_pushlightuserdata(self->L, self->waste);   lua_setglobal(self->L, "WASTE");
            } else if ( strncmp("Foundation", p->category, 10) == 0 ) {
                ArrayPush(self->foundations, p);
            } else if ( strncmp("Tableau", p->category, 7) == 0 ) {
                ArrayPush(self->tableaux, p);
            }
        }
    }

    // now the piles know their slots, calculate and set their positions
    BaizePositionPiles(self, GetScreenWidth());
}

void BaizeResetState(struct Baize *const self)
{
    if ( self->tail ) {
        ArrayFree(self->tail);
    }
    self->tail = NULL;

    if ( self->undoStack ) {
        UndoStackFree(self->undoStack);
    }
    self->undoStack = UndoStackNew();

    self->savedPosition = 0;

    self->touchedPile = NULL;
    self->touchedWidget = NULL;

    self->dragOffset = (Vector2){.x=0.0f, .y=0.0f};
    self->dragging = false;

    BaizeUndoPush(self);
}

void BaizePositionPiles(struct Baize *const self, const int windowWidth)
{
    extern float cardWidth, cardHeight, pilePaddingX, pilePaddingY, topMargin, leftMargin;

    float maxX = 0.0f;
    size_t index;
    for ( struct Pile *p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        if ( p->slot.x > maxX ) {
            maxX = p->slot.x;
        }
    }

    pilePaddingX = cardWidth / 10.0f;
    pilePaddingY = cardHeight / 10.0f;
    float w = pilePaddingX + cardWidth * (maxX + 2);
    leftMargin = ((float)windowWidth - w) / 2.0f;
    topMargin = 48.0f + (cardHeight / 10.0f);

    for ( struct Pile *p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        p->pos = PileCalculatePosFromSlot(p);
        // fprintf(stdout, "%s: %.0f, %.0f := %.0f, %.0f\n", p->category, p->slot.x, p->slot.y, p->pos.x, p->pos.y);
        PileRepushAllCards(p);
    }
}

void BaizeNewDealCommand(struct Baize *const self)
{
    // TODO record lost game if this one started

    UiHideNavDrawer(self->ui);
    BaizeCreatePiles(self);
    BaizeResetState(self);
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
            if ( CheckCollisionPointRec(pos, CardScreenRect(c)) ) {
                return c;
            }
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
    Rectangle rectCard = CardBaizeRect(c);
    size_t index;
    for ( struct Pile *p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        if ( p == c->owner ) {
            continue;
        }
        Rectangle rectPile = PileFannedBaizeRect(p);
        float area = UtilOverlapArea(rectCard, rectPile);
        if ( area > largestArea ) {
            largestArea = area;
            pile = p;
        }
    }
    return pile;
}

int BaizeCalcPercentComplete(struct Baize *const self)
{
    // int sorted = 0, unsorted = 0;
    // size_t index;
    // for ( struct Pile *p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
    //     p->vtable->CountSortedAndUnsorted(p, &sorted, &unsorted);
    // }
    // return (int)(UtilMapValue((float)sorted-(float)unsorted, -(float)self->cardsInLibrary, (float)self->cardsInLibrary, 0.0f, 100.0f));
    (void)self;
    return 42;
}

void BaizeMakeTail(struct Baize *const self, struct Card *const cFirst)
{
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

bool BaizeDragging(struct Baize *const self) {
    return !(self->dragOffset.x == 0.0f && self->dragOffset.y == 0.0f);
}

void BaizeStartDrag(struct Baize *const self) {
    // fprintf(stdout, "BaizeStartDrag\n");
    self->dragging = true;
}

void BaizeDragBy(struct Baize *const self, Vector2 delta) {
    // delta is the differnce between the point now, and what it was previously
    // (in gosol, delta is the difference between the point now and where the drag started)
    // fprintf(stdout, "BaizeDragBy %.0f, %.0f\n", delta.x, delta.y);
    self->dragOffset.x = self->dragOffset.x + delta.x;
    if ( self->dragOffset.x > 0.0f ) {
        self->dragOffset.x = 0.0f;
    }
    self->dragOffset.y = self->dragOffset.y + delta.y;
    if ( self->dragOffset.y > 0.0f ) {
        self->dragOffset.y = 0.0f;
    }
}

void BaizeStopDrag(struct Baize *const self) {
    // fprintf(stdout, "BaizeStopDrag\n");
    self->dragging = false;
}

void BaizeTouchStart(struct Baize *const self, Vector2 touchPosition)
{
    // the UI is on top of the baize, so gets first dibs
    struct Widget *w = UiFindWidgetAt(self->ui, touchPosition);
    if ( w ) {
        self->touchedWidget = w;
    } else {
        struct Card* c = findCardAt(self, touchPosition);
        if ( c ) {
            // record the distance from the card's origin to the tap point
            // dx = touchPosition.x - c->pos.x;
            // dy = touchPosition.y - c->pos.y;
            // LOGCARD(c);
            if ( !CardTransitioning(c) ) {
                BaizeMakeTail(self, c);
                if ( self->tail ) {
                    // {
                    //     size_t cindex;
                    //     for ( struct Card *cdrag = ArrayFirst(self->tail, &cindex); cdrag; cdrag = ArrayNext(self->tail, &cindex) ) {
                    //         CardStartDrag2(cdrag, cdrag->pos);
                    //     }
                    //     // center the card on the touch position (experimental)
                    //     extern float cardWidth, cardHeight;
                    //     c->pos.x = touchPosition.x - (cardWidth / 2.0);
                    //     c->pos.y = touchPosition.y - (cardHeight / 2.0);
                    // }
                    ArrayForeach(self->tail, (ArrayIterFunc)CardStartDrag);
                }
            }
        } else {
            self->touchedPile = findPileAt(self, touchPosition);    // could be NULL
        }
    }

    if ( self->tail == NULL && self->touchedPile == NULL && self->touchedWidget == NULL ) {
        BaizeStartDrag(self);
    }
    
    self->lastTouch = touchPosition;
    UiHideNavDrawer(self->ui);
}

void BaizeTouchMove(struct Baize *const self, Vector2 touchPosition)
{
    Vector2 delta = {.x = touchPosition.x - self->lastTouch.x, .y = touchPosition.y - self->lastTouch.y};

    if ( self->tail ) {
        size_t index;
        struct Card* c = (struct Card*)ArrayFirst(self->tail, &index);
        while ( c ) {
            CardMovePositionBy(c, delta);
            c = (struct Card*)ArrayNext(self->tail, &index);
        }
    } else if ( self->touchedPile ) {
        // do nothing, can't drag a pile
    } else if ( self->touchedWidget ) {
        // TODO drag widget (scroll a drawer)
    } else if ( self->dragging ) {
        BaizeDragBy(self, delta);
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
                if ( p->vtable->CanAcceptTail(self, p, self->tail) ) {
                    while ( c ) {
                        CardStopDrag(c);
                        c = (struct Card*)ArrayNext(self->tail, &index);
                    }
                    // TODO special case: dragging a card from Stock to Waste in Canfield, Klondike (Draw Three)
                    if ( PileMoveCards(p, cHeadOfTail) ) {
                        BaizeAfterUserMove(self);
                    }
                } else {
                    if ( self->errorString ) {
                        UiToast(self->ui, self->errorString);
                    }
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
        } else {    // card was not dragged
            while ( c ) {
                CardStopDrag(c);    // CardCancelDrag() would use CardTransitionTo(), and we know the card didn't move
                c = (struct Card*)ArrayNext(self->tail, &index);
            }
            if ( BaizeCardTapped(self, cHeadOfTail) ) {
                BaizeAfterUserMove(self);
            } else {
                if ( self->errorString ) {
                    UiToast(self->ui, self->errorString);
                }
            }
            // needs -C11
            // char *pt = _Generic(cHeadOfTail->owner,
            //                 struct Pile* : "Pile",
            //                 struct Tableau* : "Tableau",
            //                 default: "Other");
            // fprintf(stdout, "_Generic %s\n", pt);
        }
        ArrayFree(self->tail);
        self->tail = NULL;
    } else if ( self->touchedWidget ) {
        // fprintf(stderr, "Widget Command\n");
        if ( self->touchedWidget->bcf ) {
            struct BaizeCommand *bc = calloc(1, sizeof(struct BaizeCommand));
            if ( bc ) {
                bc->bcf = self->touchedWidget->bcf;
                ArrayPush(BaizeCommandQueue, bc);
            }
        }
        self->touchedWidget = NULL;
    } else if ( self->touchedPile ) {
        if ( self->touchedPile->vtable->PileTapped(self->touchedPile) ) {
            BaizeAfterUserMove(self);
        }
        self->touchedPile = NULL;
    } else if ( self->dragging ) {
        BaizeStopDrag(self);
    }
}

void BaizeCollectCommand(struct Baize *const self)
{
    int count, totalCount = 0;
    for (;;) {
        count = 0;
        size_t index;
        for ( struct Pile* p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
            count += p->vtable->Collect(p);
        }
        if ( count == 0 ) {
            break;
        }
        totalCount += count;
    }
    if ( totalCount > 0 ) {
        BaizeAfterUserMove(self);
    }
}

bool BaizeComplete(struct Baize *const self)
{
    size_t index;
    for ( struct Pile* p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        if ( !p->vtable->Complete(p) ) {
            return false;
        }
    }
    return true;
}

bool BaizeConformant(struct Baize *const self)
{
    size_t index;
    for ( struct Pile* p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        if ( !p->vtable->Conformant(p) ) {
            return false;
        }
    }
    return true;
}

void BaizeAfterUserMove(struct Baize *const self)
{
    // TODO automoves (in Lua)
    // TODO test started/complete/conformant

    BaizeUndoPush(self);
}

void BaizeLayout(struct Baize *const self, const int windowWidth, const int windowHeight)
{
    static int oldWidth = 0;
    static int oldHeight = 0;

    if ( oldWidth != windowWidth || oldHeight != windowHeight ) {
        BaizePositionPiles(self, windowWidth);
        UiLayout(self->ui, windowWidth, windowHeight);
        oldWidth = windowWidth;
        oldHeight = windowHeight;
    }
}

void BaizeUpdate(struct Baize *const self)
{
    Vector2 touchPosition = GetTouchPosition(0);
    int gesture = GetGestureDetected();

    switch ( gesture ) {
        case GESTURE_TAP:
            if ( !self->tail ) {
                BaizeTouchStart(self, touchPosition);
            }
            break;
        case GESTURE_DRAG:
            BaizeTouchMove(self, touchPosition);
            break;
        case GESTURE_NONE:
            if ( self->tail || self->touchedPile || self->touchedWidget || self->dragging ) {
                BaizeTouchStop(self);
            }
            break;
    }

    // static float dx, dy;
    // int dx = c.rect.x - touchPosition.x;
    // int dy = c.rect.y - touchPosition.y;
    // {
    //     char buf[64];
    //     sprintf(buf, "dragging %d,%d", dx, dy);
    //     DrawText(buf, 0, 130, 16, WHITE);
    // }
    // CardSetPos((struct Card*)ArrayGet(self->tail, 0), (Vector2){touchPosition.x - dx, touchPosition.y - dy});

    ArrayForeach(self->piles, (ArrayIterFunc)PileUpdate);

    if ( IsKeyReleased(KEY_U) ) {
        BaizeUndoCommand(self);
    }
    if ( IsKeyReleased(KEY_S) ) {
        BaizeSavePositionCommand(self);
    }
    if ( IsKeyReleased(KEY_L) ) {
        BaizeLoadPositionCommand(self);
    }
    if ( IsKeyReleased(KEY_N) ) {
        BaizeNewDealCommand(self);
    }
    if ( IsKeyReleased(KEY_R) ) {
        BaizeRestartDealCommand(self);
    }
    if ( IsKeyReleased(KEY_C) ) {
        BaizeCollectCommand(self);
    }

    UiUpdate(self->ui);
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
            if ( CardTransitioning(c) ) {
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
            if ( CardDragging(c) ) {
                CardDraw(c);
            }
            c = (struct Card*)ArrayNext(p->cards, &cindex);
        }
        p = (struct Pile*)ArrayNext(self->piles, &pindex);
    }

    // {
    //     char z[128];
    //     sprintf(z, "%.0f, %.0f", self->dragOffset.x, self->dragOffset.y);
    //     DrawText(z, 10, 100, 24, WHITE);
    // }
    // {
    //     Vector2 touchPosition = GetTouchPosition(0);
    //     c = findCardAt(self, touchPosition);
    //     if ( c ) {
    //         CardDrawRect(c, 10, 100);
    //     }
    // }
    // DrawFPS(10, 10);

    UiDraw(self->ui);

    EndDrawing();
}

void BaizeFree(struct Baize *const self)
{
    self->magic = 0;
    BaizeResetError(self);
    ArrayFree(self->tableaux);
    ArrayFree(self->foundations);
    UndoStackFree(self->undoStack);
    ArrayFree(self->tail);
    ArrayForeach(self->piles, (ArrayIterFunc)PileFree);
    ArrayFree(self->piles);
    free(self->cardLibrary);
    UiFree(self->ui);
    lua_close(self->L);
    free(self);
}

void BaizeToggleNavDrawerCommand(struct Baize *const self)
{
    UiToggleNavDrawer(self->ui);
}

size_t BaizePowerMoves(struct Baize *const self, struct Pile *const dstPile)
{
    double emptyCells = 0.0;
    double emptyCols = 0.0;
    size_t index;
    for ( struct Pile *p=ArrayFirst(self->piles, &index); p; p=ArrayNext(self->piles, &index) ) {
        if ( ArrayLen(p->cards) == 0 ) {
            if ( strcmp(p->category, "Cell") == 0 ) {
                emptyCells++;
            } else if ( strcmp(p->category, "Tableau") == 0 ) {
                // 'If you are moving into an empty column, then the column you are moving into does not count as empty column.'
                struct Tableau *t = (struct Tableau*)p;
                if ( t->accept == 0 && p != dstPile ) {
                    emptyCols++;
                }
            }
        }
    }
    double n = (1.0 + emptyCells) * pow(2.0, emptyCols);
    return (size_t)n;
}
