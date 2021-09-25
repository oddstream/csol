/* ui.c */

#include <stdio.h>
#include <string.h>

#include "array.h"

#include "ui.h"

// Toast

struct Toast* ToastNew(const char* message, int ticks)
{
    struct Toast *self = calloc(sizeof(struct Toast), 1);
    if ( self ) {
        self->message = strdup(message);
        self->ticksLeft = ticks;
    }
    return self;
}

void ToastFree(struct Toast *const self)
{
    if ( self ) {
        if ( self->message ) {
            free(self->message);
        }
        free(self);
    }
}

// ToastManager

struct ToastManager* ToastManagerNew()
{
    struct ToastManager *self = calloc(sizeof(struct ToastManager), 1);
    if ( self ) {
        self->toasts = ArrayNew(8);
    }
    return self;
}

void ToastManagerAdd(struct ToastManager *const self, struct Toast *t)
{
    // fprintf(stdout, "Adding toast %s\n", t->message);
    ArrayPush(self->toasts, t);
}

void ToastManagerUpdate(struct ToastManager *const self)
{
    if ( ArrayLen(self->toasts) == 0 ) {
        return;
    }
    size_t index;
    for ( struct Toast *t = ArrayFirst(self->toasts, &index); t; t = ArrayNext(self->toasts, &index) ) {
        t->ticksLeft--;
    }
    struct Toast *t = ArrayGet(self->toasts, 0);
    if ( t->ticksLeft < 0 ) {
        // fprintf(stdout, "Deleting toast %s\n", t->message);
        ArrayDeleteFirst(self->toasts, (ArrayFreeFunc)ToastFree);
    }
}

static void DrawCenteredText(Rectangle r, int fontSize, const char* text)
{
    int textWidth = MeasureText(text, fontSize);
    int x = ((int)(r.width / 2.0f)) - (textWidth / 2);
    int y = fontSize / 4;
    DrawText(text, (int)r.x + x, (int)r.y + y, fontSize, WHITE);
}

void ToastManagerDraw(struct ToastManager *const self)
{
    // ToastManager draws all the Toasts, rather than handing it down to the Toast object, because we know position in queue

    const float xpadding = 24.0;
    const float ypadding = 10.0f;
    const int fontSize = 32;

    int baizeWidth = GetScreenWidth();
    int baizeHeight = GetScreenHeight();

    float ty = (float)(baizeHeight - fontSize - 24);  // 24 height of statusbar
    size_t index;
    for ( struct Toast *t = ArrayFirst(self->toasts, &index); t; t = ArrayNext(self->toasts, &index) ) {
        float twidth = (float)MeasureText(t->message, fontSize) + xpadding;
        float theight = (float)fontSize * 1.5f;
        float tx = (baizeWidth / 2.0f) - (twidth / 2.0f);
        Rectangle r = {.x=tx, .y=ty, .width=twidth, .height=theight};
        DrawRectangleRounded(r, 0.05, 9, BLACK);
        DrawCenteredText(r, fontSize, t->message);

        ty = ty - r.height - ypadding;
    }
}

void ToastManagerFree(struct ToastManager *const self)
{
    if ( self ) {
        ArrayForeach(self->toasts, (ArrayIterFunc)ToastFree);
        ArrayFree(self->toasts);
        free(self);
    }
}

// UI

struct UI* UiNew(void)
{
    struct UI *self = calloc(sizeof(struct UI), 1);
    if ( self ) {
        self->toastManager = ToastManagerNew();
    }
    return self;
}

void UiToast(struct UI *const self, const char* message)
{
    struct Toast *t = ToastNew(message, 60 * (6 + ArrayLen(self->toastManager->toasts)));
    if ( t ) {
        ToastManagerAdd(self->toastManager, t);
    }
}

void UiUpdate(struct UI *const self)
{
    ToastManagerUpdate(self->toastManager);
}

void UiDraw(struct UI *const self)
{
    ToastManagerDraw(self->toastManager);
}

void UiFree(struct UI *const self)
{
    if ( self ) {
        ToastManagerFree(self->toastManager);
        free(self);
    }
}
