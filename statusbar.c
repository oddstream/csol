/* statusbar.c */

#include "ui.h"

#define HEIGHT (24.0f)

static struct ContainerVtable statusBarVtable = {
    &StatusBarLayoutWidgets,
    &StatusBarLayout,
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

void StatusBarLayoutWidgets(struct Container *const self)
{
    const float padding = (14.0f);

    size_t index;
    for ( struct Widget *w = ArrayFirst(self->widgets, &index); w; w = ArrayNext(self->widgets, &index) ) {
        switch ( w->align ) {
            case -1:
                w->rect.x = 0.0f + padding;
                break;
            case 0:
                w->rect.x = (self->rect.width / 2.0f) - (w->rect.width / 2.0f);
                break;
            case 1:
                w->rect.x = self->rect.width - w->rect.width - padding;
                break;
        }
        w->rect.y = (self->rect.height / 2.0f) - (w->rect.height / 2.0f);
    }
}

void StatusBarLayout(struct Container *const self, const int windowWidth, const int windowHeight)
{
    struct StatusBar* s = (struct StatusBar*)self;
    s->super.rect = (Rectangle){.x=0, .y=(float)windowHeight-HEIGHT, .width=(float)windowWidth, .height=HEIGHT};

    StatusBarLayoutWidgets(self);
}

void StatusBarUpdate(struct Container *const self)
{
    (void)self;
}

void StatusBarFree(struct Container *const self)
{
    // no extra members in StatusBar, so just free the base object
    ContainerFree(self);
}