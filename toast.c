/* toast.c */

#include <string.h>

#include "ui.h"

/*
 https://material.io/archive/guidelines/components/snackbars-toasts.html

	Single-line snackbar height: 48dp
	Multi-line snackbar height: 80dp
	Text: Roboto Regular 14sp
	Action button: Roboto Medium 14sp, all-caps text
	Default background fill: #323232 100%
*/

#define FONT_SIZE (14.0f)

// Toast

struct Toast* ToastNew(const char* message, int ticks)
{
    extern Font fontRobotoRegular14;
    extern float fontSpacing;

    struct Toast *self = calloc(1, sizeof(struct Toast));
    if (self) {
        self->message = strdup(message);
        self->mte = MeasureTextEx(fontRobotoRegular14, message, FONT_SIZE, fontSpacing);
        self->ticksLeft = ticks;
    }
    return self;
}

void ToastFree(struct Toast *const self)
{
    if (self) {
        if (self->message) {
            free(self->message);
        }
        free(self);
    }
}

// ToastManager

struct ToastManager* ToastManagerNew()
{
    struct ToastManager *self = calloc(1, sizeof(struct ToastManager));
    if (self) {
        self->toasts = ArrayNew(8);
    }
    return self;
}

void ToastManagerAdd(struct ToastManager *const self, struct Toast *t)
{
    // fprintf(stdout, "Adding toast %s\n", t->message);
    self->toasts = ArrayPush(self->toasts, t);
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
        ArrayDelete(self->toasts, 0, (ArrayFreeFunc)ToastFree);
    }
}

void ToastManagerDraw(struct ToastManager *const self)
{
    // ToastManager draws all the Toasts, rather than handing it down to the Toast object, because we know position in queue
    extern Color uiBackgroundColor, uiTextColor;
    extern Font fontRobotoRegular14;
    extern float fontSpacing;

    int baizeWidth = GetScreenWidth();
    int baizeHeight = GetScreenHeight();

    float rY = (float)(baizeHeight - 14 - 14 - 14 - 24);  // 24 height of statusbar
    size_t index;
    for ( struct Toast *t = ArrayFirst(self->toasts, &index); t; t = ArrayNext(self->toasts, &index) ) {
        float tX = (baizeWidth / 2.0f) - (t->mte.x / 2.0f);
        float tY = rY + (14.0f / 2.0f);
        Rectangle rText = {.x=tX, .y=tY, .width=t->mte.x, .height=t->mte.y};

        float rWidth = rText.width + 24;
        float rHeight = rText.height + 14;
        float rX = (baizeWidth / 2.0f) - (rWidth / 2.0f);

        Rectangle rRect = {.x=rX, .y=rY, .width=rWidth, .height=rHeight};

        DrawRectangleRounded(rRect, 0.05, 9, uiBackgroundColor);

        Vector2 pos = UtilCenterTextInRectangle(rRect, t->mte.x, t->mte.y);
        DrawTextEx(fontRobotoRegular14, t->message, pos, FONT_SIZE, fontSpacing, uiTextColor);

        rY = rY - rRect.height - 14.0f;
    }
}

void ToastManagerFree(struct ToastManager *const self)
{
    if (self) {
        ArrayForeach(self->toasts, (ArrayIterFunc)ToastFree);
        ArrayFree(self->toasts);
        free(self);
    }
}

