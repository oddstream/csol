/* titlebar.c */

#include "ui.h"

#define HEIGHT (36.0f)

static struct ContainerVtable titleBarVtable = {
    &TitleBarLayoutWidgets,
    &TitleBarLayout,
    &TitleBarUpdate,
    &ContainerDraw,     // no extra stuff
    &ContainerFree,     // no extra stuff
};

struct TitleBar* TitleBarNew(void)
{
    float w = (float)GetScreenWidth();

    struct TitleBar* self = calloc(1, sizeof(struct TitleBar));
    if ( self ) {
        ContainerCtor((struct Container*)self, (Rectangle){.x=0.0f, .y=0.0f, .width=w, .height=HEIGHT});
        self->super.vtable = &titleBarVtable;
    }
    return self;
}

void TitleBarLayoutWidgets(struct Container *const self)
{
    const float padding = (36.0f);

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

void TitleBarLayout(struct Container *const self, const int windowWidth, const int windowHeight)
{
    (void)windowHeight;

    struct TitleBar* tb = (struct TitleBar*)self;
    tb->super.rect = (Rectangle){.x=0, .y=0.0f, .width=(float)windowWidth, .height=HEIGHT};

    TitleBarLayoutWidgets(self);
}

void TitleBarUpdate(struct Container *const self)
{
    (void)self;
}

void TitleBarFree(struct Container *const self)
{
    // no extra members in TitleBar, so just free the base object
    ContainerFree(self);
}