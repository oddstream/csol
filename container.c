/* container.c */

#include "ui.h"

void ContainerCtor(struct Container *const self, Rectangle r)
{
    self->rect = r;
    self->widgets = ArrayNew(8);
}

Rectangle ContainerRect(struct Container *const self)
{
    return self->rect;
}

void ContainerUpdate(struct Container *const self)
{
    (void)self;
}

void ContainerDraw(struct Container *const self)
{
    extern Color uiBackgroundColor;

    DrawRectangle((int)self->rect.x, (int)self->rect.y, (int)self->rect.width, (int)self->rect.height, uiBackgroundColor);
}

void ContainerFree(struct Container *const self)
{
    if ( self ) {
        size_t index;
        for ( struct Widget *w = ArrayFirst(self->widgets, &index); w; w = ArrayNext(self->widgets, &index) ) {
            w->vtable->Free(w);
        }
        free(self);
    }
}
