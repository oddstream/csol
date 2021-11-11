/* baize.c */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <raylib.h>

#include "baize.h"
#include "moon.h"
#include "scrunch.h"
#include "undo.h"
#include "util.h"
#include "luautil.h"
#include "ui.h"

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

_Bool BaizeValid(struct Baize *const self)
{
    return self && self->magic == BAIZE_MAGIC;
}

unsigned BaizeCRC(struct Baize *const self)
{
    // calculate a CRC to detect changes to the cards
    // sizeof(unsigned) == 4 at the time of writing
    // https://stackoverflow.com/questions/21001659/crc32-algorithm-implementation-in-c-without-a-look-up-table-and-with-a-public-li
    unsigned crc = 0xFFFFFFFF, mask;
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
    if (str) self->errorString = strdup(str);
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

    lua_pushinteger(self->L, FAN_NONE);     lua_setglobal(self->L, "FAN_NONE");
    lua_pushinteger(self->L, FAN_DOWN);     lua_setglobal(self->L, "FAN_DOWN");
    lua_pushinteger(self->L, FAN_LEFT);     lua_setglobal(self->L, "FAN_LEFT");
    lua_pushinteger(self->L, FAN_RIGHT);    lua_setglobal(self->L, "FAN_RIGHT");
    lua_pushinteger(self->L, FAN_DOWN3);    lua_setglobal(self->L, "FAN_DOWN3");
    lua_pushinteger(self->L, FAN_LEFT3);    lua_setglobal(self->L, "FAN_LEFT3");
    lua_pushinteger(self->L, FAN_RIGHT3);   lua_setglobal(self->L, "FAN_RIGHT3");

    lua_createtable(self->L, 0, 0);         lua_setglobal(self->L, "Cell");
    lua_createtable(self->L, 0, 0);         lua_setglobal(self->L, "Discard");
    lua_createtable(self->L, 0, 0);         lua_setglobal(self->L, "Foundation");
    lua_createtable(self->L, 0, 0);         lua_setglobal(self->L, "Label");
    lua_createtable(self->L, 0, 0);         lua_setglobal(self->L, "Reserve");
    lua_createtable(self->L, 0, 0);         lua_setglobal(self->L, "Stock");
    lua_createtable(self->L, 0, 0);         lua_setglobal(self->L, "Tableau");
    lua_createtable(self->L, 0, 0);         lua_setglobal(self->L, "Waste");
}

void BaizeCloseLua(struct Baize *const self)
{
    if (self->L) {
        lua_close(self->L);
        self->L = NULL;
    }
}

void BaizeCreatePiles(struct Baize *const self)
{
    // reset any old piles
    if (self->piles) {
        ArrayForeach(self->piles, (ArrayIterFunc)PileFree);
        ArrayReset(self->piles);
    } else {
        self->piles = ArrayNew(8);
    }

    if (self->foundations) {
        ArrayReset(self->foundations);
    } else {
        self->foundations = ArrayNew(8);
    }
    if (self->tableaux) {
        ArrayReset(self->tableaux);
    } else {
        self->tableaux = ArrayNew(8);
    }
    self->stock = NULL;
    self->waste = NULL;

    { // scope for fname
        char fname[128];    snprintf(fname, 127, "variants/%s.lua", self->variantName);
        if ( luaL_loadfile(self->L, fname) || lua_pcall(self->L, 0, 0, 0) ) {
            fprintf(stderr, "ERROR: %s: %s\n", __func__, lua_tostring(self->L, -1));
            lua_pop(self->L, 1);
            return;
        }

        UiUpdateTitleBar(self->ui, self->variantName);
    }

    if (lua_getglobal(self->L, "BuildPiles") != LUA_TFUNCTION) { // push value of "BuildPiles" onto the stack, return type
        fprintf(stderr, "ERROR: %s: BuildPiles is not a function\n", __func__);
        lua_pop(self->L, 1);    // remove "BuildPiles" from stack
    } else {
        if (lua_pcall(self->L, 0, 0, 0) != LUA_OK) {
            fprintf(stderr, "ERROR: %s: running Lua function: %s\n", __func__, lua_tostring(self->L, -1));
            lua_pop(self->L, 1);    // remove error
        } else {
            // fprintf(stderr, "BuildPiles called ok\n");
        }
    }

    // fprintf(stderr, "%lu piles created\n", ArrayLen(self->piles));

    // setup useful shortcuts for collect (to foundations) and statusbar
    {
        // we now do this in MoonAddPile, so shortcuts are immediately available
        // (baize->stock needs to be set before MoveCard called)
        // struct Pile* p;
        // struct ArrayIterator iter = ArrayIterator(self->piles);
        // while ( (p = ArrayMoveNext(&iter)) ) {
        //     if ( strcmp("Foundation", p->category) == 0 ) {
        //         self->foundations = ArrayPush(self->foundations, p);
        //     } else if ( strcmp("Tableau", p->category) == 0 ) {
        //         self->tableaux = ArrayPush(self->tableaux, p);
        //     } else if ( strcmp(p->category, "Stock") == 0 ) {
        //         self->stock = p;
        //     } else if ( strcmp(p->category, "Waste") == 0 ) {
        //         self->waste = p;
        //     }
        // }
        if (self->stock == NULL) {
            fprintf(stderr, "ERROR: %s: no Stock\n", __func__);
        }
        if (ArrayLen(self->foundations) == 0) {
            fprintf(stderr, "WARNING: %s: no Foundations - will divide by zero\n", __func__);
        }
    }

    // now the piles know their slots, calculate and set their positions
    extern int windowWidth, windowHeight;
    BaizePositionPiles(self, windowWidth, windowHeight);
}

void BaizeResetState(struct Baize *const self, struct Array *undoStack)
{
    if ( self->tail ) {
        ArrayFree(self->tail);
    }
    self->tail = NULL;

    if ( self->undoStack ) {
        UndoStackFree(self->undoStack);
    }
    self->undoStack = undoStack ? undoStack : UndoStackNew();

    self->savedPosition = 0;

    self->touchedPile = NULL;
    self->touchedWidget = NULL;

    self->dragOffset = (Vector2){.x=0.0f, .y=0.0f};
    self->dragging = 0;
}

void BaizeGetLuaGlobals(struct Baize *const self)
{
    if (!self->stock) {
        fprintf(stderr, "ERROR: %s: Baize not formed\n", __func__);
        exit(666);
        return;
    }
    self->powerMoves = LuaUtilGetGlobalBool(self->L, "POWER_MOVES", false);
    self->stock->vtable->SetRecycles(self->stock, LuaUtilGetGlobalInt(self->L, "STOCK_RECYCLES", 32767));
}

void BaizeStartGame(struct Baize *const self)
{
#if _DEBUG
    size_t index;
    _Bool zeroPile = false;
    for ( struct Pile *p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        if ( p->pos.x == 0.0f && p->pos.y == 0.0f ) {
            zeroPile = true;
            break;
        }
    }
    if (zeroPile) fprintf(stderr, "WARNING: %s: zero pos pile\n", __func__);
#endif

    unsigned crc = BaizeCRC(self);

    if (lua_getglobal(self->L, "StartGame") != LUA_TFUNCTION) {  // push Lua function name onto the stack
        fprintf(stderr, "INFO: %s: StartGame is not a function\n", __func__);
        lua_pop(self->L, 1);  // remove function name
    } else {
        // no args, no returns
        if ( lua_pcall(self->L, 0, 0, 0) != LUA_OK ) {
            fprintf(stderr, "ERROR: %s: running Lua function: %s\n", __func__, lua_tostring(self->L, -1));
            lua_pop(self->L, 1);    // remove error
        } else {
            // nothing
        }
    }

    if (BaizeCRC(self) != crc) {
        fprintf(stdout, "INFO: %s: StartGame has changed the baize\n", __func__);
    }
}

void BaizeRefan(struct Baize *const self)
{
    size_t index;
    for ( struct Pile *p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        PileRefan(p);
    }
}

void BaizePositionPiles(struct Baize *const self, const int windowWidth, const int windowHeight)
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
    topMargin = TITLEBAR_HEIGHT + pilePaddingY;

    for ( struct Pile *p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        PileSetBaizePos(p, (Vector2){
            .x = leftMargin + (p->slot.x * (cardWidth + pilePaddingX)),
            .y = topMargin + (p->slot.y * (cardHeight + pilePaddingY)),
        });
        // fprintf(stdout, "%s: %.0f,%.0f := %.0f,%.0f\n", p->category, p->slot.x, p->slot.y, p->pos.x, p->pos.y);
    }

    BaizeCalculateScrunchLimits(self, windowWidth, windowHeight);
    // TODO compare fanFactor with default and Refan if changed
    BaizeRefan(self);

    // fprintf(stdout, "INFO: %s:\n", __func__);
}

void BaizeNewDealCommand(struct Baize *const self, void* param)
{
    (void)param;
    // TODO record lost game if this one started

    BaizeCreatePiles(self);
    BaizeResetState(self, NULL);
    BaizeStartGame(self);
    BaizeGetLuaGlobals(self);
    BaizeUndoPush(self);
}

struct Pile* BaizeFindPile(struct Baize* self, const char* category, int n)
{
    size_t index;
    struct Pile* p = ArrayFirst(self->piles, &index);
    while (p) {
        if ( strcmp(p->category, category) == 0 ) {
            n--;
            if ( n == 0 ) {
                return p;
            }
        }
        p = ArrayNext(self->piles, &index);
    }
    return NULL;
}

int BaizeCountPiles(struct Baize* self, const char* category)
{
    int count = 0;
    size_t index;
    struct Pile* p = ArrayFirst(self->piles, &index);
    while (p) {
        if ( strcmp(p->category, category) == 0 ) {
            count++;
        }
        p = ArrayNext(self->piles, &index);
    }
    return count;
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
        if ( CheckCollisionPointRec(pos, PileScreenRect(p)) ) {
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
        if ( p == CardOwner(c) ) {
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

_Bool BaizeMakeTail(struct Baize *const self, struct Card *const cFirst)
{
    struct Pile* p = CardOwner(cFirst);

    // check no cards in this pile are transitioning
    // for ( struct Card* c = ArrayFirst(p->cards, &index); c; c = ArrayNext(p->cards, &index) ) {
    //     if (CardTransitioning(c)) {
    //         return 0;
    //     }
    // }

    // find the index of the first tail card
    size_t iFirst;
    if (!ArrayIndexOf(p->cards, cFirst, &iFirst)) {
        fprintf(stderr, "ERROR: %s: card not found in pile\n", __func__);
        return 0;
    }
    // free any old tail
    if (self->tail) {
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

_Bool BaizeDragging(struct Baize *const self) {
    return !(self->dragOffset.x == 0.0f && self->dragOffset.y == 0.0f);
}

void BaizeStartDrag(struct Baize *const self) {
    // fprintf(stdout, "BaizeStartDrag\n");
    self->dragging = 1;
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
    self->dragging = 0;
}

void BaizeTouchStart(struct Baize *const self, Vector2 touchPosition)
{
    if (self->tail) {
        fprintf(stderr, "ERROR: %s: touch when there is a tail\n", __func__);
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
        if (c) {
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
        // we didn't touch a widget, so have no need for a drawer to be open, shirley?
        UiHideDrawers(self->ui);
    }

    if ( self->tail == NULL && self->touchedPile == NULL && self->touchedWidget == NULL ) {
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

static _Bool AnyTailCardsProne(struct Array *const tail)
{
    size_t index;
    for ( struct Card *c = ArrayFirst(tail, &index); c; c = ArrayNext(tail, &index) ) {
        if (CardProne(c)) {
            return 1;
        }
    }
    return 0;
}

void BaizeTouchStop(struct Baize *const self, Vector2 touchPosition)
{
    BaizeResetError(self);

    if (self->tail) {
        struct Card *c = ArrayGet(self->tail, 0);   // c is the head of the tail, the card being dragged
        if ( CardWasDragged(c) ) {
            struct Pile* p = largestIntersection(self, c);  // p is the target/destination pile
            if ( p ) {
                // fprintf(stderr, "Intersection with %s\n", p->category);
                if (!(CardOwner(c) == self->stock && p == self->waste) && AnyTailCardsProne(self->tail)) {
                    UiToast(self->ui, "(CSOL) Cannot move a face down card");
                    ArrayForeach(self->tail, (ArrayIterFunc)CardCancelDrag);
                } else {
                    if ( CardOwner(c)->vtable->CanMoveTail(self->tail) ) {
                        if (p->vtable->CanAcceptTail(self, p, self->tail)) {
                            ArrayForeach(self->tail, (ArrayIterFunc)CardStopDrag);
                            // TODO special case:
                            // dragging a card from Stock to Waste in Canfield, Klondike (Draw Three), 
                            // may trigger two cards to follow
                            if ( PileMoveCards(p, c) ) {
                                BaizeAfterUserMove(self);
                            }
                        } else {
                            // fallout from not being able to match or accept
                            if (self->errorString) {
                                UiToast(self->ui, self->errorString);
                            }
                            ArrayForeach(self->tail, (ArrayIterFunc)CardCancelDrag);
                        }
                    } else {
                        // fallout from not being able to move tail
                        if (self->errorString) {
                            UiToast(self->ui, self->errorString);
                        }
                        ArrayForeach(self->tail, (ArrayIterFunc)CardCancelDrag);
                    }
                }
            } else {
                // fprintf(stderr, "No intersection\n");
                ArrayForeach(self->tail, (ArrayIterFunc)CardCancelDrag);
            }
        } else {    // card was not dragged, ie it didn't move
            ArrayForeach(self->tail, (ArrayIterFunc)CardStopDrag);
            unsigned crc = BaizeCRC(self);
            BaizeTailTapped(self);
            BaizeResetError(self);// wipe any error messages otherwise they look a bit odd
            if (BaizeCRC(self) != crc) {
                BaizeAfterUserMove(self);
            }
            if (self->errorString) {
                UiToast(self->ui, self->errorString);
            }
            // needs -C11
            // char *pt = _Generic(c->owner,
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
                NewCommand(self->touchedWidget->bcf, self->touchedWidget->param);
            }
        }
        self->touchedWidget = NULL;
    } else if ( self->touchedPile ) {
        unsigned crc = BaizeCRC(self);
        BaizePileTapped(self, self->touchedPile);
        if (BaizeCRC(self) != crc) {
            BaizeAfterUserMove(self);
        }
        if (self->errorString) {
            UiToast(self->ui, self->errorString);
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

_Bool BaizeComplete(struct Baize *const self)
{
    size_t index;
    for ( struct Pile* p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        if ( !p->vtable->Complete(p) ) {
            return 0;
        }
    }
    return 1;
}

_Bool BaizeConformant(struct Baize *const self)
{
    size_t index;
    for ( struct Pile* p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        if ( !p->vtable->Conformant(p) ) {
            return 0;
        }
    }
    return 1;
}

void BaizeAfterUserMove(struct Baize *const self)
{
    // fprintf(stderr, "stack %d\n", lua_gettop(self->L));
    // fprintf(stdout, "Baize CRC %u\n", BaizeCRC(self));

    if (lua_getglobal(self->L, "AfterMove") != LUA_TFUNCTION) {  // push Lua function name onto the stack
        // fprintf(stderr, "INFO: %s: AfterMove is not a function\n", __func__);
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

    BaizeGetLuaGlobals(self);

    BaizeUndoPush(self);
}

void BaizeLayout(struct Baize *const self, const int newWindowWidth, const int newWindowHeight)
{
    extern int windowWidth, windowHeight;

    if ( newWindowWidth != windowWidth || newWindowHeight != windowHeight ) {
        windowWidth = newWindowWidth;
        windowHeight = newWindowHeight;
        BaizePositionPiles(self, windowWidth, windowHeight);
        UiLayout(self->ui, windowWidth, windowHeight);
    }
}

void BaizeUpdate(struct Baize *const self)
{
    Vector2 touchPosition = GetTouchPosition(0);
    int gesture = GetGestureDetected();

    /* K&R style switch formatting, see P59 if you don't believe me */
    switch (gesture) {
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

    // TODO these should go through CommandQueue rather than being called directly
    if ( IsKeyReleased(KEY_U) ) {
        BaizeUndoCommand(self, NULL);
    }
    if ( IsKeyReleased(KEY_S) ) {
        if ( IsKeyDown(KEY_LEFT_SHIFT) ) {
            BaizeSaveUndoToFile(self);
        } else {
            BaizeSavePositionCommand(self, NULL);
        }
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
#if _DEBUG
    if (IsKeyReleased(KEY_Q)) {
        ScrunchPiles(self);
    }
    if ( IsKeyReleased(KEY_Z) ) {

        size_t index;
        for ( struct Pile *p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
            PileRefan(p);
        }
    }
#endif
    UiUpdate(self->ui);
}

void BaizeDraw(struct Baize *const self)
{
    extern Color baizeColor;

    ClearBackground(baizeColor);
    BeginDrawing();

    size_t cindex, pindex;
    struct Card *c;
    struct Pile* p = ArrayFirst(self->piles, &pindex);
    while (p) {
        p->vtable->Draw(p);

        c = ArrayFirst(p->cards, &cindex);
        while (c) {
            if ( !(CardTransitioning(c) || CardDragging(c)) ) {
                CardDraw(c);
            }
            c = ArrayNext(p->cards, &cindex);
        }
        p = ArrayNext(self->piles, &pindex);
    }

    p = ArrayFirst(self->piles, &pindex);
    while (p) {
        c = ArrayFirst(p->cards, &cindex);
        while (c) {
            if (CardTransitioning(c)) {
                CardDraw(c);
            }
            c = ArrayNext(p->cards, &cindex);
        }
        p = ArrayNext(self->piles, &pindex);
    }

    p = ArrayFirst(self->piles, &pindex);
    while (p) {
        c = ArrayFirst(p->cards, &cindex);
        while (c) {
            if (CardDragging(c)) {
                CardDraw(c);
            }
            c = ArrayNext(p->cards, &cindex);
        }
        p = ArrayNext(self->piles, &pindex);
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

#if _DEBUG
    DrawFPS(10, (int)TITLEBAR_HEIGHT);
#endif

    EndDrawing();
}

void BaizeFree(struct Baize *const self)
{
    self->magic = 0;
    BaizeResetError(self);
    ArrayFree(self->foundations);
    ArrayFree(self->tableaux);
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
    BaizeCreatePiles(self);
    BaizeResetState(self, NULL);
    BaizeStartGame(self);
    BaizeGetLuaGlobals(self);
    BaizeUndoPush(self);
}

void BaizeChangeVariantCommand(struct Baize *const self, void* param)
{
    if (param) {
        strncpy(self->variantName, param, sizeof(self->variantName)-1);
        BaizeReloadVariantCommand(self, NULL);
    }
    UiHideVariantDrawer(self->ui);
}

