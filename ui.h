/* ui.h */

#ifndef UI_H
#define UI_H

#include <stdlib.h>

#include <raylib.h>

#include "array.h"
#include "command.h"
#include "baize.h"

enum IconName {
    // names taken from google material icon filenames
    NONE = -1,
    BOOKMARK = 0,
    BOOKMARK_ADD = 1,
    CHECK_BOX = 2,
    CHECK_BOX_OUTLINE_BLANK = 3,
    CLOSE = 5,
    DONE = 6,
    DONE_ALL = 7,
    INFO = 8,
    LIST = 10,
    MENU = 11,
    RADIO_BUTTON_CHECKED = 12,
    RADIO_BUTTON_UNCHECKED = 13,
    RESTORE = 15,
    SEARCH = 16,
    SETTINGS = 17,
    STAR = 18,
    UNDO = 4,
};

#define TITLEBAR_HEIGHT (48.0f)
#define STATUSBAR_HEIGHT (24.0f)
#define DRAWER_WIDTH (256.0f)
#define WIDGET_PADDING (14.0f)
#define ICON_SIZE (36.0f)

struct Container;   // forward declaration
struct Widget;
struct UI;

struct WidgetVtable {
    void (*Update)(struct Widget *const self);
    void (*Draw)(struct Widget *const self);
    void (*Free)(struct Widget *const self);
};

struct Widget {
    struct WidgetVtable *vtable;
    struct Container *parent;
    int align;  // alignment in parent container: left=-1, center=0, right=+1
    CommandFunction bcf;
    void* param;        // NULL, or a pointer to a block of memory, owned by the widget
    Rectangle rect;    // x, y relative to parent
};

void WidgetCtor(struct Widget *const self, struct Container* parent, int align, CommandFunction bcf, void* param);
Rectangle WidgetScreenRect(struct Widget *const self);
void WidgetUpdate(struct Widget *const self);
void WidgetDraw(struct Widget *const self);
void WidgetFree(struct Widget *const self);

struct IconWidget {
    struct Widget super;
    enum IconName frame;
};

struct IconWidget* IconWidgetNew(struct Container *parent, int align, enum IconName frame, CommandFunction cf, void* param);
void IconWidgetDraw(struct Widget *const self);

struct TextWidget {
    struct Widget super;
    enum IconName frame;
    Font *font;
    float fontSize;
    char *text;
};

struct TextWidget* TextWidgetNew(struct Container *parent, enum IconName frame, Font *font, float fontSize, int align, CommandFunction cf, void* param);
void TextWidgetSetText(struct TextWidget *const self, const char *text);
void TextWidgetDraw(struct Widget *const self);
void TextWidgetFree(struct Widget *const self);

struct ContainerVtable {
    // Rectangle (*Rect)(struct Container *const self);  // screen coords
    // struct Widget* (*FindWidgetAt)(struct Container *const self, Vector2 pos);
    void (*StartDrag)(struct Container *const self, Vector2 pos);
    void (*DragBy)(struct Container *const self, Vector2 delta);
    void (*StopDrag)(struct Container *const self, Vector2 pos);
    _Bool (*WasDragged)(struct Container *const self, Vector2 pos);
    // _Bool (*Visible)(struct Container *const self);
    // void (*Show)(struct Container *const self);
    // void (*Hide)(struct Container *const self);
    void (*Layout)(struct Container *const self, const int windowWidth, const int windowHeight);
    void (*Update)(struct Container *const self);
    void (*Draw)(struct Container *const self);
    void (*Free)(struct Container *const self);
};

struct Container {
    struct ContainerVtable *vtable;
    Rectangle rect; // x, y screen coords
    struct Array *widgets;
};

void ContainerCtor(struct Container *const self, Rectangle r);
// void ContainerLayoutWidgets(struct Container *const self);
void ContainerStartDrag(struct Container *const self, Vector2 pos);
void ContainerDragBy(struct Container *const self, Vector2 delta);
void ContainerStopDrag(struct Container *const self, Vector2 pos);
_Bool ContainerWasDragged(struct Container *const self, Vector2 pos);
void ContainerUpdate(struct Container *const self);
void ContainerDraw(struct Container *const self);
void ContainerFree(struct Container *const self);

enum DrawerAniState {
    LEFT = -1,
    STOP = 0,
    RIGHT = 1,
};

struct Drawer {
    struct Container super;
    enum DrawerAniState aniState;
    Vector2 dragOffset;
    Vector2 dragStartPos;
};

void DrawerCtor(struct Drawer *const self, Rectangle r);
void DrawerStartDrag(struct Container *const self, Vector2 pos);
void DrawerDragBy(struct Container *const self, Vector2 delta);
void DrawerStopDrag(struct Container *const self, Vector2 pos);
_Bool DrawerWasDragged(struct Container *const self, Vector2 pos);
void DrawerLayoutWidgets(struct Container *const self);
void DrawerLayout(struct Container *const self, const int windowWidth, const int windowHeight);
void DrawerUpdate(struct Container *const self);
void DrawerDraw(struct Container *const self);
void DrawerFree(struct Container *const self);

_Bool DrawerVisible(struct Drawer *const self);
void DrawerHide(struct Drawer *const self);
void DrawerShow(struct Drawer *const self);

struct NavDrawer {
    struct Drawer super;
};

struct NavDrawer* NavDrawerNew(void);
void NavDrawerFree(struct Container *const self);

struct StatusBar {
    struct Container super;
};

struct StatusBar* StatusBarNew(void);
Rectangle StatusBarRect(struct Container *const self);
void StatusBarLayoutWidgets(struct Container *const self);
void StatusBarLayout(struct Container *const self, const int windowWidth, const int windowHeight);
void StatusBarUpdate(struct Container *const self);
void StatusBarFree(struct Container *const self);

struct TitleBar {
    struct Container super;
};

struct TitleBar* TitleBarNew(void);
Rectangle TitleBarRect(struct Container *const self);
void TitleBarLayoutWidgets(struct Container *const self);
void TitleBarLayout(struct Container *const self, const int windowWidth, const int windowHeight);
void TitleBarUpdate(struct Container *const self);
void TitleBarFree(struct Container *const self);

struct Toast {
    char *message;  // strdup'd
    Vector2 mte;    // cached result from MeasureTextEx
    int ticksLeft;
};

struct Toast* ToastNew(const char* message, int ticks);
void ToastFree(struct Toast *const self);

struct ToastManager {
    struct Array *toasts;
};

struct ToastManager* ToastManagerNew(void);
void ToastManagerAdd(struct ToastManager *const self, struct Toast *t);
void ToastManagerUpdate(struct ToastManager *const self);
void ToastManagerDraw(struct ToastManager *const self);
void ToastManagerFree(struct ToastManager *const self);


struct UI* UiNew(void);
void UiToast(struct UI *const self, const char* message);
void UiUpdateStatusBar(struct UI *const self, const char* left, const char* center, const char *right);
void UiUpdateTitleBar(struct UI *const self, const char* center);

void UiShowNavDrawer(struct UI *const self);
void UiHideNavDrawer(struct UI *const self);
void UiToggleNavDrawer(struct UI *const self);
void UiShowVariantDrawer(struct UI *const self);
void UiHideVariantDrawer(struct UI *const self);
void UiToggleVariantDrawer(struct UI *const self);
void UiHideDrawers(struct UI *const self);

struct Container* UiFindContainerAt(struct UI *const self, Vector2 pos);
struct Widget* UiFindWidgetAt(struct UI *const self, Vector2 pos);
void UiLayout(struct UI *const self, const int windowWidth, const int windowHeight);
void UiUpdate(struct UI *const self);
void UiDraw(struct UI *const self);
void UiFree(struct UI *const self);

struct UI {
    struct Array *containers;

    // handy shortcuts to save digging around in container array
    struct TitleBar *titleBar;
    struct StatusBar *statusBar;
    struct NavDrawer *navDrawer;
    struct NavDrawer *variantDrawer;

    struct ToastManager *toastManager;
};

#endif
