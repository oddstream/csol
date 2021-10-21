/* baize.h */

#ifndef BAIZE_H
#define BAIZE_H

#include <lua.h>

#include "array.h"
#include "pile.h"

struct Baize {
    unsigned magic;

    char variantName[64];

    size_t numberOfCardsInSuit;
    size_t numberOfCardsInLibrary;
    struct Card *cardLibrary;

    lua_State *L;

    struct Array *piles;
    struct Pile *stock;         // a struct Stock*
    struct Pile *waste;         // a struct Waste*, may be NULL, used by status bar
    struct Array *foundations;  // used by collect

    struct Array *tail;
    struct Array *undoStack;
    size_t savedPosition;
    Vector2 lastTouch;
    struct Pile *touchedPile;

    struct Widget *touchedWidget;

    bool dragging;
    Vector2 dragOffset;

    bool powerMoves;    // set from POWERMOVES variable in variant.lua

    struct UI* ui;

    char* errorString;
};

struct Array* BaizeCommandQueue;
typedef void (*BaizeCommandFunction)(struct Baize *const, void* param);
struct BaizeCommand {
    // ISO C forbids conversion of object pointer to function pointer type [-Werror=pedantic]
    // so we hide our function pointer in a struct
    BaizeCommandFunction bcf;
    void *param;
};

struct Baize* BaizeNew(const char*);
bool BaizeValid(struct Baize *const self);
unsigned int BaizeCRC(struct Baize *const self);
void BaizeResetError(struct Baize *const self);
void BaizeSetError(struct Baize *const self, const char *str);
void BaizeOpenLua(struct Baize *const self);
void BaizeCloseLua(struct Baize *const self);
void BaizeCreatePiles(struct Baize *const self);
void BaizeResetState(struct Baize *const self, struct Array *undoStack);
void BaizePositionPiles(struct Baize *const self, const int windowWidth);
void BaizeNewDealCommand(struct Baize *const self, void* param);
struct Pile* BaizeFindPile(struct Baize* self, const char *category, int n);
bool BaizeMakeTail(struct Baize *const self, struct Card *const cFirst);
bool BaizeDragging(struct Baize *const self);
void BaizeStartDrag(struct Baize *const self);
void BaizeDragBy(struct Baize *const self, Vector2 delta);
void BaizeStopDrag(struct Baize *const self);
void BaizeTouchStart(struct Baize *const self, Vector2 touchPosition);
void BaizeTouchMove(struct Baize *const self, Vector2 touchPosition);
void BaizeTouchStop(struct Baize *const self, Vector2 touchPosition);
void BaizeCollectCommand(struct Baize *const self, void* param);
bool BaizeComplete(struct Baize *const self);
bool BaizeConformant(struct Baize *const self);
void BaizeAfterUserMove(struct Baize *const self);
void BaizeLayout(struct Baize *const self, const int width, const int height);
void BaizeUpdate(struct Baize *const self);
void BaizeDraw(struct Baize *const self);
void BaizeFree(struct Baize *const self);

bool BaizeCardTapped(struct Baize *const self, struct Card *const c);
bool BaizePileTapped(struct Baize *const self, struct Pile *const c);
void BaizeStartGame(struct Baize *const self);
void BaizeToggleNavDrawerCommand(struct Baize *const self, void* param);
void BaizeToggleVariantDrawerCommand(struct Baize *const self, void* param);
void BaizeFindGameCommand(struct Baize *const self, void* param);
void BaizeReloadVariantCommand(struct Baize *const self, void* param);
void BaizeChangeVariantCommand(struct Baize *const self, void* param);

#endif
