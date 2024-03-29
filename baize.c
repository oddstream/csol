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
#include "luautil.h"
#include "scrunch.h"
#include "tableau.h"
#include "trace.h"
#include "ui.h"
#include "undo.h"
#include "util.h"

#define BAIZE_MAGIC (0x19910920)

struct Baize* BaizeNew(const char *packName)
{
    struct Baize* self = calloc(1, sizeof(struct Baize));
    if (!self) {
        return NULL;
    }
    self->magic = BAIZE_MAGIC;
    self->pack = PackCtor(packName);
    if (!self->pack) {
        self->pack = PackCtor("default"); // try a fallback
        if (!self->pack) {
            free(self);
            return NULL;
        }
    }
    self->ui = UiNew();	// TODO move to TheUI global

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
    if (str) self->errorString = UtilStrDup(str);
}

void BaizeResetError(struct Baize *const self)
{
    if (self->errorString) {
        free(self->errorString);
        self->errorString = NULL;
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

    if (self->discards) {
        ArrayReset(self->discards);
    } else {
        self->discards = ArrayNew(8);
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

    self->script = GetInterface(self);

    self->script->BuildPiles(self);

    // fprintf(stderr, "%lu piles created\n", ArrayLen(self->piles));

    if (self->stock == NULL) {
        CSOL_ERROR("%s", "no Stock");
    }
    if (ArrayLen(self->foundations) == 0) {
        CSOL_WARNING("%s", "no Foundations - will divide by zero");
    }

    // now the piles know their slots, calculate and set their positions
    BaizeFindBuddyPiles(self);
    BaizeLayoutCommand(self, NULL);
    UiUpdateTitleBar(self->ui, self->variantName);
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

    self->bookmark = 0;

    self->touchedPile = NULL;
    self->touchedWidget = NULL;

    self->dragOffset = (Vector2){0};
    self->dragging = 0;
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
    extern float pilePaddingX, pilePaddingY, topMargin, leftMargin;

    float maxX = 0.0f;
    size_t index;
    for ( struct Pile *p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        if ( p->slot.x > maxX ) {
            maxX = p->slot.x;
        }
    }

    pilePaddingX = self->pack->width / 10.0f;
    pilePaddingY = self->pack->height / 10.0f;
    float w = pilePaddingX + self->pack->width * (maxX + 1.5);
    leftMargin = ((float)windowWidth - w) / 2.0f;
    topMargin = TITLEBAR_HEIGHT + pilePaddingY;

    for ( struct Pile *p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        PileSetBaizePos(p, (Vector2){
            .x = leftMargin + (p->slot.x * (self->pack->width + pilePaddingX)),
            .y = topMargin + (p->slot.y * (self->pack->height + pilePaddingY)),
        });
        // fprintf(stdout, "%s: %.0f,%.0f := %.0f,%.0f\n", p->category, p->slot.x, p->slot.y, p->pos.x, p->pos.y);
    }

    BaizeCalculateScrunchDims(self, windowWidth, windowHeight);

    BaizeRefan(self);
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
        CSOL_ERROR("card %d %d not found in pile", cFirst->id.ordinal, cFirst->id.suit);
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
    BaizeCalculateScrunchDims(self, GetScreenWidth(), GetScreenHeight());
    ScrunchPiles(self);
}

void BaizeTouchStart(struct Baize *const self, Vector2 touchPosition)
{
    if (self->tail) {
        CSOL_ERROR("%s", "touch when there is a tail");
    }

    // the UI is on top of the baize, so gets first dibs
    struct Widget *w = UiFindWidgetAt(self->ui, touchPosition);
    if (w) {
        // the Widget's container is w->parent
        w->parent->vtable->StartDrag(w->parent, touchPosition);
        self->touchedWidget = w;
    } else {
        struct Card* c = findCardAt(self, touchPosition);
        if (c) {
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
            // {    char z[64]; CardToString(ArrayGet(self->tail, 0), z);   fprintf(stdout, "Touched card %s\n", z);   }
            ArrayForeach(self->tail, (ArrayIterFunc)CardStopDrag);
            unsigned crc = BaizeCRC(self);
            self->script->TailTapped(self->tail);
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
                PostCommand(self->touchedWidget->bcf, self->touchedWidget->param);
            }
        }
        self->touchedWidget = NULL;
    } else if ( self->touchedPile ) {
        unsigned crc = BaizeCRC(self);
        self->script->PileTapped(self->touchedPile);
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

void BaizeAfterUserMove(struct Baize *const self)
{
    // fprintf(stderr, "stack %d\n", lua_gettop(L));
    // fprintf(stdout, "Baize CRC %u\n", BaizeCRC(self));

    self->script->AfterMove(self);

    // fprintf(stderr, "stack %d\n", lua_gettop(L));

    if ( BaizeComplete(self) ) {
        UiToast(self->ui, "Game complete");
    }

    BaizeUndoPush(self);
}

void BaizeLayoutCommand(struct Baize *const self, void *param)
{
    (void)param;

    int w = GetScreenWidth();
    int h = GetScreenHeight();

    // CSOL_INFO("resizing to %d,%d", w, h);

    BaizePositionPiles(self, w, h);
    UiLayout(self->ui, w, h);
}

void BaizeLayout(struct Baize *const self)
{
    (void)self;

    if (IsWindowResized()) {
        PostUniqueCommand(BaizeLayoutCommand, NULL);
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
    if ( IsKeyReleased(KEY_BACKSPACE) ) {
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
        BaizeFindVariantCommand(self, NULL);
    }
    if ( IsKeyReleased(KEY_C) ) {
        BaizeCollectCommand(self, NULL);
    }
#ifdef _DEBUG
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
    if (IsKeyReleased(KEY_ZERO)) {
        BaizeChangePackCommand(self, "default");
    } else if (IsKeyReleased(KEY_TWO)) {
        BaizeChangePackCommand(self, "unicode");
    } else if (IsKeyReleased(KEY_FOUR)) {
        BaizeChangePackCommand(self, "fourcolor");
    }

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

#ifdef _DEBUG
    if (GetFPS()<60) {
        DrawFPS(10, (int)TITLEBAR_HEIGHT);
    }
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
    PackDtor(self->pack);
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

void BaizeFindVariantCommand(struct Baize *const self, void* param)
{
    (void)param;
    UiHideNavDrawer(self->ui);
    UiShowVariantDrawer(self->ui);
}

void BaizeNewDealCommand(struct Baize *const self, void* param)
{
    (void)param;
    // TODO record lost game if this one started

    BaizeResetState(self, NULL);
    size_t index;
    for ( struct Pile* p = ArrayFirst(self->piles, &index); p; p = ArrayNext(self->piles, &index) ) {
        p->vtable->Reset(p);
    }
    self->script->StartGame(self);
    BaizeUndoPush(self);
}

// RestartDealCommand is in undo.c, because it's just a big undo

void BaizeReloadVariantCommand(struct Baize *const self, void* param)
{
    (void)param;
    // don't want global variables from one variant being carried over into next variant
    // there isn't a luaL_reset, so...
    CloseLua();
    OpenLua(self);
    BaizeCreatePiles(self);
    BaizeResetState(self, NULL);
    self->script->StartGame(self);
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

void BaizeChangePackCommand(struct Baize *const baize, void* param)
{
    if (param) {
        struct Pack *pack = PackCtor(param);
        if (pack) {
            PackDtor(baize->pack);
            baize->pack = pack;
            BaizeLayoutCommand(baize, NULL);
        }
    }
}

void BaizeWikipediaCommand(struct Baize *const baize, void* param)
{
    (void)param;
    const char *str = baize->script->Wikipedia();
    CSOL_INFO("Wikipedia '%s'", str ? str : "(null)");
    if (str) {
        char buff[512];
        // sprintf(buff, "xdg-open \"%s\"", str);
        // https://askubuntu.com/questions/8252/how-to-launch-default-web-browser-from-the-terminal
        sprintf(buff, "URL=\"%s\"; xdg-open $URL || sensible-browser $URL || x-www-browser $URL || gnome-open $URL", str);
        int result = system(buff);
        CSOL_INFO("system returned %d", result);
    }
}
