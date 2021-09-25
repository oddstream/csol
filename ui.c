/* ui.c */

#include <stdio.h>
#include <string.h>

#include "array.h"

#include "ui.h"

/*
 https://material.io/archive/guidelines/components/snackbars-toasts.html

	Single-line snackbar height: 48dp
	Multi-line snackbar height: 80dp
	Text: Roboto Regular 14sp
	Action button: Roboto Medium 14sp, all-caps text
	Default background fill: #323232 100%
*/

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

static void DrawCenteredText(Rectangle r, const char* text, Vector2 mte)
{
    extern Font fontRoboto14;
    extern Color uiTextColor;

    Vector2 pos;
    pos.x = r.x + (r.width / 2.0f) - (mte.x / 2.0f);
    pos.y = r.y + (r.height / 2.0f) - (mte.y / 2.0f);
    DrawTextEx(fontRoboto14, text, pos, 14.0f, 1.0f, uiTextColor);
}

void ToastManagerDraw(struct ToastManager *const self)
{
    // ToastManager draws all the Toasts, rather than handing it down to the Toast object, because we know position in queue
    extern Color uiBackgroundColor;
    extern Font fontRoboto14;

    int baizeWidth = GetScreenWidth();
    int baizeHeight = GetScreenHeight();

    float rY = (float)(baizeHeight - 14 - 14 - 24);  // 24 height of statusbar
    size_t index;
    for ( struct Toast *t = ArrayFirst(self->toasts, &index); t; t = ArrayNext(self->toasts, &index) ) {
        Vector2 mte = MeasureTextEx(fontRoboto14, t->message, 14.0f, 1.0f);
        // mte.x += 24.0f;
        // mte.y += 14.0f;

        float tX = (baizeWidth / 2.0f) - (mte.x / 2.0f);
        float tY = rY + (14.0f / 2.0f);
        Rectangle rText = {.x=tX, .y=tY, .width=mte.x, .height=mte.y};

        float rWidth = rText.width + 24;
        float rHeight = rText.height + 14;
        float rX = (baizeWidth / 2.0f) - (rWidth / 2.0f);

        Rectangle rRect = {.x=rX, .y=rY, .width=rWidth, .height=rHeight};

        DrawRectangleRounded(rRect, 0.05, 9, uiBackgroundColor);
        DrawCenteredText(rText, t->message, mte);

        rY = rY - rRect.height - 14.0f;
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
