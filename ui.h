/* ui.h */

#ifndef UI_H
#define UI_H

#include <stdlib.h>

#include <raylib.h>

#include "array.h"

struct Container;   // forward declaration
struct Widget;
struct UI;

struct WidgetVtable {
    struct Container* (*Parent)(struct Widget *const self);  // returns Container this widget belongs to
    Rectangle (*Rect)(struct Widget *const self);  // returns x,y (relative to parent), width, height
    // Rectangle (*OffsetRect)(struct Widget *const self);  // gives the screen position in relation to parent's position
    void (*SetPosition)(struct Widget *const self, Vector2); // sets position rleative to parent
    int (*Align)(struct Widget *const self); // returns alignment (-1, 0, 1)
    void (*Update)(struct Widget *const self);
    void (*Draw)(struct Widget *const self);
    void (*Free)(struct Widget *const self);
    // NotifyCallback
};

struct Widget {
    struct WidgetVtable *vtable;
    struct Container *parent;
    int align;  // alignment in parent container: left=-1, center=0, right=+1
    // bool disabled;
    Rectangle rect;    // x, y relative to parent
};

void WidgetCtor(struct Widget *const self, struct Container* parent, int align);
struct Container* WidgetParent(struct Widget *const self);
struct Rectangle WidgetRect(struct Widget *const self);
void WidgetSetPosition(struct Widget *const self, Vector2 pos);
int WidgetAlign(struct Widget *const self);
void WidgetUpdate(struct Widget *const self);
void WidgetDraw(struct Widget *const self);
void WidgetFree(struct Widget *const self);

struct TextWidget {
    struct Widget super;
    char *text;
};

struct ContainerVtable {
    Rectangle (*Rect)(struct Container *const self);  // screen coords
    // struct Widget* (*FindWidgetAt)(struct Container *const self, Vector2 pos);
    // void (*LayoutWidgets)(struct Container *const self);
    // bool (*StartDrag)(struct Container *const self, Vector2 pos);
    // void (*DragBy)(struct Container *const self, Vector2 delta);
    // void (*StopDrag)(struct Container *const self);
    // bool (*Visible)(struct Container *const self);
    // void (*Show)(struct Container *const self);
    // void (*Hide)(struct Container *const self);
    void (*Update)(struct Container *const self);
    void (*Draw)(struct Container *const self);
    void (*Free)(struct Container *const self);
};

struct Container {
    struct ContainerVtable *vtable;
    Rectangle rect;
    struct Array *widgets;
};

void ContainerCtor(struct Container *const self, Rectangle r);
Rectangle ContainerRect(struct Container *const self);
void ContainerUpdate(struct Container *const self);
void ContainerDraw(struct Container *const self);
void ContainerFree(struct Container *const self);

struct StatusBar {
    struct Container super;
};

struct StatusBar* StatusBarNew(void);
Rectangle StatusBarRect(struct Container *const self);
void StatusBarUpdate(struct Container *const self);
void StatusBarDraw(struct Container *const self);
void StatusBarFree(struct Container *const self);

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
    struct Array *containers;
    struct ToastManager *toastManager;
};

#endif
