/* ui.c */

#include <stdio.h>
#include <string.h>

#include "ui.h"
#include "undo.h"

static int cmpfunc(const void* a, const void *b) {
    char * const * aa = a;
    char * const * bb = b;
    return strcmp(*aa, *bb);
}

#if 0
static void qsortfiles(char** files, int count) {
    // Sort files by name
    // https://github.com/raysan5/raygui/blob/master/examples/custom_file_dialog/gui_file_dialog.h
    // https://en.wikibooks.org/wiki/Algorithm_Implementation/Sorting/Quicksort#C
    if (count > 1)
    {
        const unsigned int MAX = 64;
        unsigned int left = 0, stack[64], pos = 0, seed = rand(), len = count;

        for (;;)
        {
            for (; left + 1 < len; len++)    // Sort left to len - 1
            {
                if (pos == MAX) len = stack[pos = 0];               // Stack overflow, reset
                char *pivot = files[left + seed%(len - left)];      // Pick random pivot
                seed = seed*69069 + 1;                              // Next pseudo-random number
                stack[pos++] = len;                                 // Sort right part later

                for (unsigned int right = left - 1;;)               // Inner loop: partitioning
                {
                    while (strcmp(files[++right], pivot) < 0);      // Look for greater element
                    while (strcmp(pivot, files[--len]) < 0);        // Look for smaller element
                    if (right >= len) break;                        // Partition point found?
                    char *temp = files[right];
                    files[right] = files[len];                      // The only swap
                    files[len] = temp;
                }                                                   // Partitioned, continue left part
            }

            if (pos == 0) break;                                    // Stack empty?
            left = len;                                             // Left to right is sorted
            len = stack[--pos];                                     // Get next range to sort
        }
    }
}
#endif

struct UI* UiNew(void)
{
    extern Font fontRobotoRegular14, fontRobotoMedium24;

    struct UI *self = calloc(1, sizeof(struct UI));
    if ( self ) {
        self->containers = ArrayNew(8);

        self->titleBar = TitleBarNew();
        if ( self->titleBar ) {
            struct IconWidget *iw = IconWidgetNew((struct Container*)self->titleBar, -1, MENU, BaizeToggleNavDrawerCommand, NULL);
            if ( iw ) {
                ArrayPush(((struct Container*)self->titleBar)->widgets, iw);
            }
            struct TextWidget *tw = TextWidgetNew((struct Container*)self->titleBar, NONE, &fontRobotoMedium24, 24.0f, 0, NULL, NULL);
            if ( tw ) {
                TextWidgetSetText(tw, "Variant Title");
                ArrayPush(((struct Container*)self->titleBar)->widgets, tw);
            }
            iw = IconWidgetNew((struct Container*)self->titleBar, 1, UNDO, BaizeUndoCommand, NULL);
            if ( iw ) {
                ArrayPush(((struct Container*)self->titleBar)->widgets, iw);
            }
            iw = IconWidgetNew((struct Container*)self->titleBar, 1, DONE, BaizeCollectCommand, NULL);
            if ( iw ) {
                ArrayPush(((struct Container*)self->titleBar)->widgets, iw);
            }
            ArrayPush(self->containers, self->titleBar);
            TitleBarLayoutWidgets((struct Container*)self->titleBar);
        }

        self->statusBar = StatusBarNew();
        if ( self->statusBar ) {
            struct TextWidget *tw = TextWidgetNew((struct Container*)self->statusBar, NONE, &fontRobotoRegular14, 14.0f, -1, NULL, NULL);
            if ( tw ) {
                TextWidgetSetText(tw, "STOCK:");
                ArrayPush(((struct Container*)self->statusBar)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)self->statusBar, NONE, &fontRobotoRegular14, 14.0f, 0, NULL, NULL);
            if ( tw ) {
                TextWidgetSetText(tw, "MOVES");
                ArrayPush(((struct Container*)self->statusBar)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)self->statusBar, NONE, &fontRobotoRegular14, 14.0f, 1, NULL, NULL);
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
            struct TextWidget *tw = TextWidgetNew((struct Container*)self->navDrawer, STAR, &fontRobotoMedium24, 24.0f, -1, BaizeNewDealCommand, NULL);
            if ( tw ) {
                TextWidgetSetText(tw, "New deal");
                ArrayPush(((struct Container*)self->navDrawer)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)self->navDrawer, RESTORE, &fontRobotoMedium24, 24.0f, -1, BaizeRestartDealCommand, NULL);
            if ( tw ) {
                TextWidgetSetText(tw, "Restart deal");
                ArrayPush(((struct Container*)self->navDrawer)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)self->navDrawer, SEARCH, &fontRobotoMedium24, 24.0f, -1, BaizeFindGameCommand, NULL);
            if ( tw ) {
                TextWidgetSetText(tw, "Find game...");
                ArrayPush(((struct Container*)self->navDrawer)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)self->navDrawer, BOOKMARK_ADD, &fontRobotoMedium24, 24.0f, -1, BaizeSavePositionCommand, NULL);
            if ( tw ) {
                TextWidgetSetText(tw, "Bookmark");
                ArrayPush(((struct Container*)self->navDrawer)->widgets, tw);
            }
            tw = TextWidgetNew((struct Container*)self->navDrawer, BOOKMARK, &fontRobotoMedium24, 24.0f, -1, BaizeLoadPositionCommand, NULL);
            if ( tw ) {
                TextWidgetSetText(tw, "Goto bookmark");
                ArrayPush(((struct Container*)self->navDrawer)->widgets, tw);
            }

            ArrayPush(self->containers, self->navDrawer);
        }

        self->variantDrawer = NavDrawerNew();
        if (self->variantDrawer) {
            int count = 0;
            char **files = GetDirectoryFiles("variants", &count);
            if ( count ) {

                qsort(files, count, sizeof(char*), cmpfunc);
                // qsortfiles(files, count);

                for ( int i=0; i<count; i++ ) {
                    // fprintf(stdout, "FILE: %s\n", files[i]);
                    if (IsFileExtension(files[i], ".lua")) {
                        const char* vname = GetFileNameWithoutExt(files[i]);
                        if (vname[0] == '~') {
                            continue;
                        }
                        struct TextWidget *tw = TextWidgetNew((struct Container*)self->variantDrawer, NONE, &fontRobotoMedium24, 24.0f, -1, BaizeChangeVariantCommand, strdup(vname));
                        if ( tw ) {
                            TextWidgetSetText(tw, GetFileNameWithoutExt(files[i]));
                            ArrayPush(((struct Container*)self->variantDrawer)->widgets, tw);
                        }
                    }
                }
            }
            ClearDirectoryFiles();
            ArrayPush(self->containers, self->variantDrawer);
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

void UiHideDrawers(struct UI *const self)
{
    if ( DrawerVisible((struct Drawer*)self->navDrawer) ) {
        DrawerHide((struct Drawer*)self->navDrawer);
    }
    if ( DrawerVisible((struct Drawer*)self->variantDrawer) ) {
        DrawerHide((struct Drawer*)self->variantDrawer);
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

void UiToggleVariantDrawer(struct UI *const self)
{
    if ( DrawerVisible((struct Drawer*)self->variantDrawer) ) {
        DrawerHide((struct Drawer*)self->variantDrawer);
    } else {
        DrawerShow((struct Drawer*)self->variantDrawer);
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
