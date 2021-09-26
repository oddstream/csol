/* container.c */

#include "ui.h"

void ContainerCtor(struct Container *const self, Rectangle r)
{
    self->rect = r;
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
        free(self);
    }
}
