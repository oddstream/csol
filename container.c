/* container.c */

#include "ui.h"

void ContainerCtor(struct Container *const self, Rectangle r)
{
    self->rect = r;
    self->widgets = ArrayNew(8);
}

void ContainerLayoutWidgets(struct Container *const self)
{
    (void)self; // all laying out will be done by subclasses
}

void ContainerUpdate(struct Container *const self)
{
    (void)self; // all updating done by subclass
}

void ContainerDraw(struct Container *const self)
{
    extern Color uiBackgroundColor;

    DrawRectangle((int)self->rect.x, (int)self->rect.y, (int)self->rect.width, (int)self->rect.height, uiBackgroundColor);

    size_t index;
    for ( struct Widget *w = ArrayFirst(self->widgets, &index); w; w = ArrayNext(self->widgets, &index) ) {
        w->vtable->Draw(w);
    }
}

void ContainerFree(struct Container *const self)
{
    if ( self ) {
        size_t index;
        for ( struct Widget *w = ArrayFirst(self->widgets, &index); w; w = ArrayNext(self->widgets, &index) ) {
            w->vtable->Free(w);
        }
        ArrayFree(self->widgets);
        free(self);
    }
}
