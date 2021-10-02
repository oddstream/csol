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

        self->titleBar = TitleBarNew();
        if ( self->titleBar ) {
            struct IconWidget *iw = IconWidgetNew((struct Container*)self->titleBar, -1, MENU, BaizeToggleNavDrawerCommand);
            if ( iw ) {
                ArrayPush(((struct Container*)self->titleBar)->widgets, iw);
            }
            struct TextWidget *tw = TextWidgetNew((struct Container*)self->titleBar, NONE, &fontRobotoMedium24, 24.0f, 0, NULL);
            if ( tw ) {
                TextWidgetSetText(tw, "Variant Title");
                ArrayPush(((struct Container*)self->titleBar)->widgets, tw);
            }
            iw = IconWidgetNew((struct Container*)self->titleBar, 1, UNDO, BaizeUndoCommand);
            if ( iw ) {
                ArrayPush(((struct Container*)self->titleBar)->widgets, iw);
            }
            ArrayPush(self->containers, self->titleBar);
            TitleBarLayoutWidgets((struct Container*)self->titleBar);
        }

        self->statusBar = StatusBarNew();
        if ( self->statusBar ) {
            struct TextWidget *tw = TextWidgetNew((struct Container*)self->statusBar, NONE, &fontRobotoRegular14, 14.0f, -1, NULL);
            if ( tw ) {
                TextWidgetSetText(tw, "STOCK:");
                ArrayPush(((struct Container*)self->statusBar)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)self->statusBar, NONE, &fontRobotoRegular14, 14.0f, 0, NULL);
            if ( tw ) {
                TextWidgetSetText(tw, "MOVES");
                ArrayPush(((struct Container*)self->statusBar)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)self->statusBar, NONE, &fontRobotoRegular14, 14.0f, 1, NULL);
            if ( tw ) {
                TextWidgetSetText(tw, "PERCENT COMPLETE");
                ArrayPush(((struct Container*)self->statusBar)->widgets, tw);
            }

            ArrayPush(self->containers, self->statusBar);
            StatusBarLayoutWidgets((struct Container*)self->statusBar);
        }

/*
		NewNavItem(n, "star", "New deal", ebiten.KeyN),
		NewNavItem(n, "restore", "Restart deal", ebiten.KeyR),
		NewNavItem(n, "search", "Find game...", ebiten.KeyF),
		NewNavItem(n, "bookmark_add", "Bookmark", ebiten.KeyS),
		NewNavItem(n, "bookmark", "Goto bookmark", ebiten.KeyL),
		NewNavItem(n, "list", "Rules...", ebiten.KeyF1),
		NewNavItem(n, "info", "Statistics...", ebiten.KeyF4),
		NewNavItem(n, "settings", "Settings...", ebiten.KeyF3),
*/

        self->navDrawer = NavDrawerNew();
        if ( self->navDrawer ) {
            struct TextWidget *tw = TextWidgetNew((struct Container*)self->navDrawer, STAR, &fontRobotoMedium24, 24.0f, -1, BaizeNewDealCommand);
            if ( tw ) {
                TextWidgetSetText(tw, "New deal");
                ArrayPush(((struct Container*)self->navDrawer)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)self->navDrawer, RESTORE, &fontRobotoMedium24, 24.0f, -1, BaizeRestartDealCommand);
            if ( tw ) {
                TextWidgetSetText(tw, "Restart deal");
                ArrayPush(((struct Container*)self->navDrawer)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)self->navDrawer, BOOKMARK_ADD, &fontRobotoMedium24, 24.0f, -1, BaizeSavePositionCommand);
            if ( tw ) {
                TextWidgetSetText(tw, "Bookmark");
                ArrayPush(((struct Container*)self->navDrawer)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)self->navDrawer, BOOKMARK, &fontRobotoMedium24, 24.0f, -1, BaizeLoadPositionCommand);
            if ( tw ) {
                TextWidgetSetText(tw, "Goto bookmark");
                ArrayPush(((struct Container*)self->navDrawer)->widgets, tw);
            }

            ArrayPush(self->containers, self->navDrawer);
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
    struct TextWidget *tw = ArrayGet(((struct Container*)self->titleBar)->widgets, 1); // TODO parameterize
    TextWidgetSetText(tw, center);
}

void UiHideNavDrawer(struct UI *const self)
{
    if ( DrawerVisible((struct Drawer*)self->navDrawer) ) {
        DrawerHide((struct Drawer*)self->navDrawer);
    }
}

void UiToggleNavDrawer(struct UI *const self)
{
    if ( DrawerVisible((struct Drawer*)self->navDrawer) ) {
        DrawerHide((struct Drawer*)self->navDrawer);
    } else {
        DrawerShow((struct Drawer*)self->navDrawer);
    }
}

void UiUpdateStatusBar(struct UI *const self, const char* left, const char* center, const char *right)
{
    struct TextWidget *tw = ArrayGet(((struct Container*)self->statusBar)->widgets, 0); // TODO parameterize
    TextWidgetSetText(tw, left);
    tw = ArrayGet(((struct Container*)self->statusBar)->widgets, 1); // TODO parameterize
    TextWidgetSetText(tw, center);
    tw = ArrayGet(((struct Container*)self->statusBar)->widgets, 2); // TODO parameterize
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
