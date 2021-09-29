/* ui.c */

#include <stdio.h>

#include "ui.h"
#include "undo.h"

struct UI* UiNew(void)
{
    extern Font fontRobotoRegular14, fontRobotoMedium24;

    struct UI *self = calloc(1, sizeof(struct UI));
    if ( self ) {
        self->containers = ArrayNew(8);

        struct TitleBar *tb = TitleBarNew();
        if ( tb ) {
            struct IconWidget *iw = IconWidgetNew((struct Container*)tb, -1, MENU, NULL);
            if ( iw ) {
                ArrayPush(((struct Container*)tb)->widgets, iw);
            }
            struct TextWidget *tw = TextWidgetNew((struct Container*)tb, &fontRobotoMedium24, 24.0f, 0);
            if ( tw ) {
                TextWidgetSetText(tw, "Variant Title");
                ArrayPush(((struct Container*)tb)->widgets, tw);
            }
            iw = IconWidgetNew((struct Container*)tb, 1, UNDO, BaizeUndoCommand);
            if ( iw ) {
                ArrayPush(((struct Container*)tb)->widgets, iw);
            }
            ArrayPush(self->containers, tb);
            TitleBarLayoutWidgets((struct Container*)tb);
        }

        struct StatusBar *sb = StatusBarNew();
        if ( sb ) {
            struct TextWidget *tw = TextWidgetNew((struct Container*)sb, &fontRobotoRegular14, 14.0f, -1);
            if ( tw ) {
                TextWidgetSetText(tw, "STOCK:");
                ArrayPush(((struct Container*)sb)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)sb, &fontRobotoRegular14, 14.0f, 0);
            if ( tw ) {
                TextWidgetSetText(tw, "MOVES");
                ArrayPush(((struct Container*)sb)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)sb, &fontRobotoRegular14, 14.0f, 1);
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
    if ( message == NULL || message[0] == '\0' ) {
        fprintf(stderr, "WARNING: empty toast\n");
        return;
    }
    struct Toast *const t = ToastNew(message, 60 * (6 + ArrayLen(self->toastManager->toasts)));
    if ( t ) {
        ToastManagerAdd(self->toastManager, t);
    }
}

void UiUpdateTitleBar(struct UI *const self, const char* center)
{
    struct Container *const tb = ArrayGet(self->containers, 0); // TODO parameterize
    struct TextWidget *tw = ArrayGet(tb->widgets, 1); // TODO parameterize
    TextWidgetSetText(tw, center);
}

void UiUpdateStatusBar(struct UI *const self, const char* left, const char* center, const char *right)
{
    struct Container *const sb = ArrayGet(self->containers, 1); // TODO parameterize
    struct TextWidget *tw = ArrayGet(sb->widgets, 0); // TODO parameterize
    TextWidgetSetText(tw, left);
    tw = ArrayGet(sb->widgets, 1); // TODO parameterize
    TextWidgetSetText(tw, center);
    tw = ArrayGet(sb->widgets, 2); // TODO parameterize
    TextWidgetSetText(tw, right);
}

struct Widget* UiFindWidgetAt(struct UI *const self, Vector2 pos)
{
    size_t cindex, windex;
    for ( struct Container *con = ArrayFirst(self->containers, &cindex); con; con = ArrayNext(self->containers, &cindex) ) {
        if ( CheckCollisionPointRec(pos, con->rect) ) {
            // fprintf(stdout, "Tapped on a container\n");
            for ( struct Widget *w = ArrayFirst(con->widgets, &windex); w; w = ArrayNext(con->widgets, &windex) ) {
                Rectangle r = w->rect;
                r.x += con->rect.x;
                r.y += con->rect.y;
                if ( CheckCollisionPointRec(pos, r) ) {
                    // fprintf(stdout, "Tapped on a widget\n");
                    return w;
                }
            }
        }
    }
    return NULL;
}

void UiLayout(struct UI *const self, const int windowWidth, const int windowHeight)
{
    size_t index;
    for ( struct Container *con = ArrayFirst(self->containers, &index); con; con = ArrayNext(self->containers, &index) ) {
        con->vtable->Layout(con, windowWidth, windowHeight);
    }
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
