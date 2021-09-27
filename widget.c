/* widget.c */

#include "ui.h"

void WidgetCtor(struct Widget *const self, struct Container* parent, int align)
{
    self->parent = parent;
    self->align = align;
}

Rectangle WidgetRect(struct Widget *const self)
{
    return self->rect;
}

void WidgetUpdate(struct Widget *const self)
{
    (void)self;
}

void WidgetDraw(struct Widget *const self)
{
    (void)self;
}

void WidgetFree(struct Widget *const self)
{
    if ( self ) {
        free(self);
    }
}
