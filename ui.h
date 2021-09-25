/* ui.h */

#ifndef UI_H
#define UI_H

#include <stdlib.h>

#include <raylib.h>

struct Container;   // forward declaration
struct Widget;
struct UI;

struct WidgetVtable {
    struct Container (*Parent)(struct Widget *const self);
    Rectangle (*Rect)(struct Widget *const self);
    Rectangle (*OffsetRect)(struct Widget *const self);  // gives the screen position in relation to parent's position
    void (*SetPosition)(struct Widget *const self, Vector2);
    int (*Align)(struct Widget *const self);
    bool (*Disabled)(struct Widget *const self);
    void (*Activate)(struct Widget *const self);
    void (*Deactivate)(struct Widget *const self);
    void (*Update)(struct Widget *const self);
    void (*Draw)(struct Widget *const self);
    // NotifyCallback
};

struct Widget {
    struct WidgetVtable *vtable;
    struct Container *parent;
    // reference to a spritesheet icon
    int align;  // left=-1, center=0, right=+1
    bool disabled;
    Rectangle rect;    // x, y relative to parent
};

struct ContainerVtable {
    Rectangle (*Rect)(struct Widget *const self);  // screen coords
    struct Widget* (*FindWidgetAt)(Vector2 pos);
    void (*LayoutWidgets)(struct Widget *const self);
    bool (*StartDrag)(Vector2 pos);
    void (*DragBy)(Vector2 delta);
    void (*StopDrag)(struct Widget *const self);
    bool (*Visible)(struct Widget *const self);
    void (*Show)(struct Widget *const self);
    void (*Hide)(struct Widget *const self);
    void (*Update)(struct Widget *const self);
    void (*Draw)(struct Widget *const self);
};

void WidgetCtor(struct Widget *const self);

struct Toast {
    char *message;  // strdup'd
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
void UiUpdate(struct UI *const self);
void UiDraw(struct UI *const self);
void UiFree(struct UI *const self);

struct UI {
    struct ToastManager *toastManager;
};

#endif
