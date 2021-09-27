/* widget.c */

#include "ui.h"

void WidgetCtor(struct Widget *const self, struct Container* parent, int align)
{
    self->parent = parent;
    self->align = align;
}

struct Container* WidgetParent(struct Widget *const self)
{
    return self->parent;
}

Rectangle WidgetRect(struct Widget *const self)
{
    return self->rect;
}

void WidgetSetPosition(struct Widget *const self, Vector2 pos)
{
    self->rect.x = pos.x;
    self->rect.y = pos.y;
}

int WidgetAlign(struct Widget *const self)
{
    return self->align;
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
