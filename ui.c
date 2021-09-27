/* ui.c */

#include "ui.h"

struct UI* UiNew(void)
{
    struct UI *self = calloc(sizeof(struct UI), 1);
    if ( self ) {
        self->containers = ArrayNew(8);

        struct StatusBar *sb = StatusBarNew();
        if ( sb ) {
            struct TextWidget *tw = TextWidgetNew((struct Container*)sb, -1);
            if ( tw ) {
                TextWidgetSetText(tw, "STOCK:");
                ArrayPush(((struct Container*)sb)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)sb, 0);
            if ( tw ) {
                TextWidgetSetText(tw, "MOVES");
                ArrayPush(((struct Container*)sb)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)sb, 1);
            if ( tw ) {
                TextWidgetSetText(tw, "PERCENT COMPLETE");
                ArrayPush(((struct Container*)sb)->widgets, tw);
            }

            ArrayPush(self->containers, sb);
            StatusBarLayoutWidgets((struct Container*)sb);
        }
        self->toastManager = ToastManagerNew();
    }
    return self;
}

void UiToast(struct UI *const self, const char* message)
{
    struct Toast *const t = ToastNew(message, 60 * (6 + ArrayLen(self->toastManager->toasts)));
    if ( t ) {
        ToastManagerAdd(self->toastManager, t);
    }
}

void UiUpdateStatusBar(struct UI *const self, const char* left, const char* center, const char *right)
{
    struct Container *const sb = ArrayGet(self->containers, 0); // TODO parameterize
    struct TextWidget *tw = ArrayGet(sb->widgets, 0); // TODO parameterize
    TextWidgetSetText(tw, left);
    tw = ArrayGet(sb->widgets, 1); // TODO parameterize
    TextWidgetSetText(tw, center);
    tw = ArrayGet(sb->widgets, 2); // TODO parameterize
    TextWidgetSetText(tw, right);
}

void UiUpdate(struct UI *const self)
{
    size_t index;
    for ( struct Container *con = ArrayFirst(self->containers, &index); con; con = ArrayNext(self->containers, &index) ) {
        con->vtable->Update(con);
    }
    ToastManagerUpdate(self->toastManager);
}

void UiDraw(struct UI *const self)
{
    size_t index;
    for ( struct Container *con = ArrayFirst(self->containers, &index); con; con = ArrayNext(self->containers, &index) ) {
        con->vtable->Draw(con);
    }
    ToastManagerDraw(self->toastManager);
}

void UiFree(struct UI *const self)
{
    if ( self ) {
        size_t index;
        for ( struct Container *con = ArrayFirst(self->containers, &index); con; con = ArrayNext(self->containers, &index) ) {
            con->vtable->Free(con);
        }
        ArrayFree(self->containers);
        ToastManagerFree(self->toastManager);
        free(self);
    }
}
