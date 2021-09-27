/* statusbar.c */

#include "ui.h"

#define HEIGHT (24.0f)

static struct ContainerVtable statusBarVtable = {
    &StatusBarRect,
    &StatusBarLayoutWidgets,
    &StatusBarUpdate,
    &ContainerDraw,     // no extra stuff
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

void StatusBarLayoutWidgets(struct Container *const self)
{
    const float padding = (14.0f);

    size_t index;
    for ( struct Widget *w = ArrayFirst(self->widgets, &index); w; w = ArrayNext(self->widgets, &index) ) {
        Vector2 pos;
        switch ( w->align ) {
            case -1:
                pos.x = 0.0f + padding;
                break;
            case 0:
                pos.x = (self->rect.width / 2.0f) - (w->rect.width / 2.0f);
                break;
            case 1:
                pos.x = self->rect.width - w->rect.width - padding;
                break;
        }
        pos.y = (self->rect.height / 2.0f) - (w->rect.height / 2.0f);
        WidgetSetPosition(w, pos);
    }
}

void StatusBarUpdate(struct Container *const self)
{
    // ContainerUpdate(self);
    if ( IsWindowResized() )
    {
        float w = (float)GetScreenWidth();
        float h = (float)GetScreenHeight();
        struct StatusBar* s = (struct StatusBar*)self;
        s->super.rect = (Rectangle){.x=0, .y=h-HEIGHT, .width=w, .height=HEIGHT};

        StatusBarLayoutWidgets(self);
    }
}

void StatusBarFree(struct Container *const self)
{
    // no extra members in StatusBar, so just free the base object
    ContainerFree(self);
}