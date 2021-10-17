/* baize.c */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "csol.h"

#define BAIZE_MAGIC (0x19910920)

struct Baize* BaizeNew(const char *variant)
{
    struct Baize* self = calloc(1, sizeof(struct Baize));
    if ( !self ) {
        return NULL;
    }
    self->magic = BAIZE_MAGIC;
    strcpy(self->variantName, variant);
    self->ui = UiNew();

    return self;
}

bool BaizeValid(struct Baize *const self)
{
    return self && self->magic == BAIZE_MAGIC;
}

unsigned int BaizeCRC(struct Baize *const self)
{
    // calculate a CRC to detect changes to the cards
    // sizeof(unsigned int) == 4 at the time of writing
    // https://stackoverflow.com/questions/21001659/crc32-algorithm-implementation-in-c-without-a-look-up-table-and-with-a-public-li
    unsigned int crc = 0xFFFFFFFF, mask;
    size_t pindex;
    for ( struct Pile* p = ArrayFirst(self->piles, &pindex); p; p = ArrayNext(self->piles, &pindex) ) {
        crc = crc ^ ArrayLen(p->cards);
        for ( int j = 7; j >= 0; j-- ) {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
    }
    return ~crc;
}

void BaizeSetError(struct Baize *const self, const char *str)
{
    BaizeResetError(self);
    self->errorString = strdup(str);
}

void BaizeResetError(struct Baize *const self)
{
    if (self->errorString) {
        free(self->errorString);
        self->errorString = NULL;
    }
}

void BaizeOpenLua(struct Baize *const self)
{
    // nb there isn't a luaL_resetstate() so any globals set in one variant end up in the next
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
}

void BaizeCloseLua(struct Baize *const self)
{
    if (self->L) {
        lua_close(self->L);
        self->L = NULL;
    }
}

void BaizeCreateCards(struct Baize *const self)
{
    // the Baize object has been created by BaizeNew
    // or has been used to play another variant
    // reset/create piles Array
    // open variant.lua, get PACKS, create cardLibrary

    // TODO record lost game if current game started

    { // scope for fname
        char fname[128];

        snprintf(fname, 127, "variants/%s.lua", self->variantName);

        if ( luaL_loadfile(self->L, fname) || lua_pcall(self->L, 0, 0, 0) ) {
            fprintf(stderr, "ERROR: %s: %s\n", __func__, lua_tostring(self->L, -1));
            lua_pop(self->L, 1);
            return;
        }

        UiUpdateTitleBar(self->ui, self->variantName);
    }

    bool cardFilter[14] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};

    {
        if ( lua_getglobal(self->L, "STRIP_CARDS") != LUA_TTABLE ) {
            fprintf(stdout, "STRIP_CARDS is not set\n");
        } else {
            fprintf(stdout, "STRIP_CARDS is set\n");
            for ( int i=1; i<14; i++ ) {
                lua_pushinteger(self->L, i);    // pushes +1 onto stack
                lua_gettable(self->L, -2);      // pops integer/index, pushes STRIP_CARDS[i]
                if ( lua_isnumber(self->L, -1) ) {
                    int result;
                    result = lua_tointeger(self->L, -1);    // doesn't alter stack
                    if ( result > 0 && result < 14 ) {
                        cardFilter[result] = 0;
                    } else {
                        fprintf(stderr, "ERROR: STRIP_CARDS: invalid value %d\n", result);
                    }
                }
                lua_pop(self->L, 1);    // remove result of lua_gettable
            }
        }
        lua_pop(self->L, 1);    // remove result of lua_getglobal

        self->numberOfCardsInSuit = 0;
        for ( int i=1; i<14; i++ ) {
            if (cardFilter[i]) {
                self->numberOfCardsInSuit += 1;
            }
        }
    }

    {   // scope for packs, suits, cardRequired, i
        size_t packs = MoonGetGlobalInt(self->L, "PACKS", 1);
        size_t suits = MoonGetGlobalInt(self->L, "SUITS", 4);
        size_t cardsRequired = packs * suits * self->numberOfCardsInSuit;

        // first time this is called, self->cardLibrary will be NULL because we used calloc()
        if (self->cardLibrary) {
            // memset(self->cardLibrary, 0, self->cardsInLibrary * sizeof(struct Card));
            free(self->cardLibrary);
        }
        self->cardLibrary = calloc(cardsRequired, sizeof(struct Card));
        self->numberOfCardsInLibrary = cardsRequired;

        size_t i = 0;
        for ( size_t pack = 0; pack < packs; pack++ ) {
            for ( enum CardOrdinal o = ACE; o <= KING; o++ ) {
                for ( enum CardSuit s = 0; s < suits; s++ ) {
                    if ( cardFilter[o] ) {
                        self->cardLibrary[i++] = CardNew(pack, o, s);
                    } else {
                        // fprintf(stderr, "Skipping %u\n", o);
                    }
                }
            }
        }
        self->numberOfCardsInLibrary = i;   // incase any were taken out by cardFilter

        fprintf(stdout, "%s: packs=%lu, suits=%lu, cards created=%lu\n", __func__, packs, suits, self->numberOfCardsInLibrary);
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
        self->piles = ArrayPush(self->piles, self->stock);
        for ( size_t i = 0; i < self->numberOfCardsInLibrary; i++ ) {
            PilePushCard(self->stock, &self->cardLibrary[i]);
        }
        unsigned int seed = MoonGetGlobalInt(self->L, "SEED", time(NULL) & 0xFFFF);
        srand(seed);
        // Knuth-Fisher–Yates shuffle
        size_t n = ArrayLen(self->stock->cards);
        for ( int i = n-1; i > 0; i-- ) {
            int j = rand() % (i+1);
            ArraySwap(self->stock->cards, i, j);
        }
    }

    // fprintf(stderr, "stock has %lu cards\n", PileLen(self->stock));

    int typ = lua_getglobal(self->L, "Build");  // push value of "Build" onto the stack
    if ( typ != LUA_TFUNCTION ) {
        fprintf(stderr, "ERROR: %s: Build is not a function\n", __func__);
        lua_pop(self->L, 1);    // remove "Build" from stack
    } else {
        if ( lua_pcall(self->L, 0, 0, 0) != LUA_OK ) {
            fprintf(stderr, "ERROR: %s: running Lua function: %s\n", __func__, lua_tostring(self->L, -1));
            lua_pop(self->L, 1);    // remove error
        } else {
            // fprintf(stderr, "Build called ok\n");
        }
    }

    // fprintf(stderr, "%lu piles created\n", ArrayLen(self->piles));

    {
        if ( self->foundations ) {
            ArrayReset(self->foundations);
        } else {
            self->foundations = ArrayNew(8);
        }
        self->waste = NULL;

        size_t pindex;
        for ( struct Pile *p = ArrayFirst(self->piles, &pindex); p; p = ArrayNext(self->piles, &pindex) ) {
            if ( strcmp("Foundation", p->category) == 0 ) {
                self->foundations = ArrayPush(self->foundations, p);
            } else if ( strcmp(p->category, "Waste") == 0 ) {
                self->waste = p;
            }
        }
        if (self->waste) {
            lua_pushlightuserdata(self->L, self->waste);
            lua_setglobal(self->L, "WASTE");
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

void BaizeNewDealCommand(struct Baize *const self, void* param)
{
    (void)param;
    // TODO record lost game if this one started

    UiHideDrawers(self->ui);
    BaizeCreatePiles(self);
    BaizeResetState(self);
    BaizeStartGame(self);
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

bool BaizeMakeTail(struct Baize *const self, struct Card *const cFirst)
{
    size_t index = 0;
    struct Pile* p = cFirst->owner;

    // check no cards in this pile are transitioning
    // for ( struct Card* c = ArrayFirst(p->cards, &index); c; c = ArrayNext(p->cards, &index) ) {
    //     if (CardTransitioning(c)) {
    //         return false;
    //     }
    // }

    // find the index of the first tail card
    size_t iFirst = 9999;
    for ( struct Card* c = ArrayFirst(p->cards, &index); c; c = ArrayNext(p->cards, &index) ) {
        if (c == cFirst) {
            iFirst = index;
            break;
        }
    }
    if (iFirst == 999) {
        fprintf(stderr, "ERROR: %s: card not found in pile\n", __func__);
        return false;
    }
    // free any old tail
    if ( self->tail ) {
        ArrayFree(self->tail);
        self->tail = NULL;
    }
    // make a new tail
    self->tail = ArrayNew(ArrayCap(p->cards));
    if (self->tail) {
        ArrayCopyTail(self->tail, p->cards, iFirst);
    }
    return ArrayLen(self->tail) > 0;
}

bool BaizeDragging(struct Baize *const self) {
    return !(self->dragOffset.x == 0.0f && self->dragOffset.y == 0.0f);
}

void BaizeStartDrag(struct Baize *const self) {
    // fprintf(stdout, "BaizeStartDrag\n");
    self->dragging = true;
}

void BaizeDragBy(struct Baize *const self, Vector2 delta) {
    // delta is the difference between the point now, and what it was previously
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
    if ( self->tail ) {
        fprintf(stderr, "ERROR: touch when there is a tail\n");
    }

    // the UI is on top of the baize, so gets first dibs
    // the Widget's container is at w->parent
    struct Widget *w = UiFindWidgetAt(self->ui, touchPosition);
    if (w) {
        // ContainerStartDrag(w->parent, touchPosition);
        w->parent->vtable->StartDrag(w->parent, touchPosition);
        self->touchedWidget = w;
    } else {
        struct Card* c = findCardAt(self, touchPosition);
        if ( c ) {
            // record the distance from the card's origin to the tap point
            // dx = touchPosition.x - c->pos.x;
            // dy = touchPosition.y - c->pos.y;
            // LOGCARD(c);
            if ( BaizeMakeTail(self, c) ) {
                ArrayForeach(self->tail, (ArrayIterFunc)CardStartDrag);
            }
        } else {
            self->touchedPile = findPileAt(self, touchPosition);    // could be NULL
        }
    }

    if ( self->tail == NULL && self->touchedPile == NULL && self->touchedWidget == NULL ) {
        UiHideDrawers(self->ui);
        BaizeStartDrag(self);
    }
    
    self->lastTouch = touchPosition;
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
        // ContainerDragBy(self->touchedWidget->parent, delta);
        self->touchedWidget->parent->vtable->DragBy(self->touchedWidget->parent, delta);
    } else if ( self->dragging ) {
        BaizeDragBy(self, delta);
    }

    self->lastTouch = touchPosition;
}

void BaizeTouchStop(struct Baize *const self, Vector2 touchPosition)
{
    BaizeResetError(self);

    if (self->tail) {
        // TODO this could be complicated and optimized by using CanAcceptCard() & PileMoveCard(dst,src) if len tail == 1
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
                    if (self->errorString) {
                        UiToast(self->ui, self->errorString);
                    }
                    while (c) {
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
                if (self->errorString) {
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
        self->touchedWidget->parent->vtable->StopDrag(self->touchedWidget->parent, touchPosition);
        if (!self->touchedWidget->parent->vtable->WasDragged(self->touchedWidget->parent, touchPosition)) {
            // fprintf(stderr, "Widget Command\n");
            if (self->touchedWidget->bcf) {
                struct BaizeCommand *bc = calloc(1, sizeof(struct BaizeCommand));
                if ( bc ) {
                    bc->bcf = self->touchedWidget->bcf;
                    bc->param = self->touchedWidget->param;
                    BaizeCommandQueue = ArrayPush(BaizeCommandQueue, bc);
                }
            }
        }
        self->touchedWidget = NULL;
    } else if ( self->touchedPile ) {
        if ( BaizePileTapped(self, self->touchedPile) ) {
            BaizeAfterUserMove(self);
        } else {
            if (self->errorString) {
                UiToast(self->ui, self->errorString);
            }
        }
        self->touchedPile = NULL;
    } else if ( self->dragging ) {
        BaizeStopDrag(self);
    }
}

void BaizeCollectCommand(struct Baize *const self, void* param)
{
    (void)param;

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
    // fprintf(stderr, "stack %d\n", lua_gettop(self->L));
    // fprintf(stdout, "Baize CRC %u\n", BaizeCRC(self));

    int typ = lua_getglobal(self->L, "AfterMove");  // push Lua function name onto the stack
    if ( typ != LUA_TFUNCTION ) {
        // fprintf(stderr, "AfterMove is not a function\n");
        lua_pop(self->L, 1);  // remove func from stack
    } else {
        // no args, no returns
        if ( lua_pcall(self->L, 0, 0, 0) != LUA_OK ) {
            fprintf(stderr, "ERROR: %s: running Lua function: %s\n", __func__, lua_tostring(self->L, -1));
            lua_pop(self->L, 1);    // remove error
        } else {
            // nothing
        }
    }

    // fprintf(stderr, "stack %d\n", lua_gettop(self->L));

    if ( BaizeComplete(self) ) {
        UiToast(self->ui, "Game complete");
    }
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
                BaizeTouchStop(self, touchPosition);
            }
            break;
        default:
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
        BaizeUndoCommand(self, NULL);
    }
    if ( IsKeyReleased(KEY_S) ) {
        BaizeSavePositionCommand(self, NULL);
    }
    if ( IsKeyReleased(KEY_L) ) {
        BaizeLoadPositionCommand(self, NULL);
    }
    if ( IsKeyReleased(KEY_N) ) {
        BaizeNewDealCommand(self, NULL);
    }
    if ( IsKeyReleased(KEY_R) ) {
        if ( IsKeyDown(KEY_LEFT_SHIFT) ) {
            BaizeReloadVariantCommand(self, NULL);
        } else {
            BaizeRestartDealCommand(self, NULL);
        }
    }
    if ( IsKeyReleased(KEY_F) ) {
        BaizeFindGameCommand(self, NULL);
    }
    if ( IsKeyReleased(KEY_C) ) {
        BaizeCollectCommand(self, NULL);
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
    ArrayFree(self->foundations);
    UndoStackFree(self->undoStack);
    ArrayFree(self->tail);
    ArrayForeach(self->piles, (ArrayIterFunc)PileFree);
    ArrayFree(self->piles);
    free(self->cardLibrary);
    UiFree(self->ui);
    free(self);
}

void BaizeToggleNavDrawerCommand(struct Baize *const self, void* param)
{
    (void)param;
    UiToggleNavDrawer(self->ui);
}

void BaizeToggleVariantDrawerCommand(struct Baize *const self, void* param)
{
    (void)param;
    UiToggleVariantDrawer(self->ui);
}

void BaizeFindGameCommand(struct Baize *const self, void* param)
{
    (void)param;
    UiHideNavDrawer(self->ui);
    UiShowVariantDrawer(self->ui);
}

void BaizeReloadVariantCommand(struct Baize *const self, void* param)
{
    (void)param;
    // don't want global variables from one variant being carried over into next variant
    // there isn't a luaL_reset, so...
    BaizeCloseLua(self);
    BaizeOpenLua(self);
    BaizeCreateCards(self);
    BaizeCreatePiles(self);
    BaizeResetState(self);
    BaizeStartGame(self);
}

void BaizeChangeVariantCommand(struct Baize *const self, void* param)
{
    if (param) {
        strncpy(self->variantName, param, sizeof(self->variantName)-1);
        BaizeReloadVariantCommand(self, NULL);
    }
    UiHideVariantDrawer(self->ui);
}

