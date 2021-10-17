/* titlebar.c */

#include "ui.h"

static struct ContainerVtable titleBarVtable = {
    &ContainerStartDrag, // NOOP
    &ContainerDragBy,    // NOOP
    &ContainerStopDrag,  // NOOP
    &ContainerWasDragged,// NOOP
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
        ContainerCtor((struct Container*)self, (Rectangle){.x=0.0f, .y=0.0f, .width=w, .height=TITLEBAR_HEIGHT});
        self->super.vtable = &titleBarVtable;
    }
    return self;
}

void TitleBarLayoutWidgets(struct Container *const self)
{
    const float padding = (36.0f);

    float nextLeft = 0.0f + padding;
    float nextRight = self->rect.width - padding;

    size_t index;
    for ( struct Widget *w = ArrayFirst(self->widgets, &index); w; w = ArrayNext(self->widgets, &index) ) {
        switch ( w->align ) {
            case -1:
                w->rect.x = nextLeft;
                nextLeft += w->rect.width + padding;
                break;
            case 0:
                w->rect.x = (self->rect.width / 2.0f) - (w->rect.width / 2.0f);
                break;
            case 1:
                w->rect.x = nextRight - w->rect.width;
                nextRight -= w->rect.width + padding;
                break;
            default:
                break;
        }
        w->rect.y = (self->rect.height / 2.0f) - (w->rect.height / 2.0f);
    }
}

void TitleBarLayout(struct Container *const self, const int windowWidth, const int windowHeight)
{
    (void)windowHeight;

    struct TitleBar* tb = (struct TitleBar*)self;
    tb->super.rect = (Rectangle){.x=0, .y=0.0f, .width=(float)windowWidth, .height=TITLEBAR_HEIGHT};

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