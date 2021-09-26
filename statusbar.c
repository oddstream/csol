/* statusbar.c */

#include "ui.h"

#define HEIGHT (24.0f)

static struct ContainerVtable statusBarVtable = {
    &StatusBarRect,
    &StatusBarUpdate,
    &StatusBarDraw,
    &StatusBarFree,
};

struct StatusBar* StatusBarNew(void)
{
    float w = (float)GetScreenWidth();
    float h = (float)GetScreenHeight();

    struct StatusBar* self = calloc(1, sizeof(struct StatusBar));
    if ( self ) {
        ContainerCtor((struct Container*)self, (Rectangle){.x=0, .y=h-HEIGHT, .width=w, .height=HEIGHT});
        self->super.vtable = &statusBarVtable;
    }
    return self;
}

Rectangle StatusBarRect(struct Container *const self)
{
    return ContainerRect(self);
}

void StatusBarUpdate(struct Container *const self)
{
    ContainerUpdate(self);
    if ( IsWindowResized() )
    {
        float w = (float)GetScreenWidth();
        float h = (float)GetScreenHeight();
        struct StatusBar* s = (struct StatusBar*)self;
        s->super.rect = (Rectangle){.x=0, .y=h-HEIGHT, .width=w, .height=HEIGHT};
    }
}

void StatusBarDraw(struct Container *const self)
{
    ContainerDraw(self);
    // TODO draw widgets
}

void StatusBarFree(struct Container *const self)
{
    // no extra members in StatusBar, so just free the base object
    ContainerFree(self);
}