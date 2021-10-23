/* widget.c */

#include "baize.h"
#include "command.h"
#include "ui.h"

void WidgetCtor(struct Widget *const self, struct Container* parent, int align, CommandFunction cf, void* param)
{
    self->parent = parent;
    self->align = align;
    self->bcf = cf;
    self->param = param;
}

Rectangle WidgetScreenRect(struct Widget *const self)
{
    Rectangle r;
    r.x = self->parent->rect.x + self->rect.x;
    r.y = self->parent->rect.y + self->rect.y;
    r.width = self->rect.width;
    r.height = self->rect.height;
    return r;
}

void WidgetUpdate(struct Widget *const self)
{
    (void)self;
}

void WidgetDraw(struct Widget *const self)
{
    (void)self; // all drawing done by subclasses
}

void WidgetFree(struct Widget *const self)
{
    if (self) {
        if (self->param) {
            free(self->param);
        }
        free(self);
    }
}
