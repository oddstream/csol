/* iconwidget.c */

#include "spritesheet.h"
#include "ui.h"

static struct WidgetVtable iconWidgetVtable = {
    &WidgetUpdate,  // nothing to update, so use super update
    &IconWidgetDraw,
    &WidgetFree,    // nothing extra to free, so use base free
};

struct IconWidget* IconWidgetNew(struct Container *parent, int align, enum IconName frame)
{
    struct IconWidget* self = calloc(1, sizeof(struct IconWidget));
    if ( self ) {
        WidgetCtor((struct Widget*)self, parent, align);
        self->super.vtable = &iconWidgetVtable;
        self->frame = frame;
    }
    return self;
}

void IconWidgetDraw(struct Widget *const self)
{
    extern struct Spritesheet *ssIcons;

    struct IconWidget *const iw = (struct IconWidget*)self;

    // get the container's screen rect
    struct Container *const con = self->parent;
    Rectangle rect = con->rect;
    // add our x,y to the container's x,y to get screen position
    Rectangle iconRect;
    iconRect.x = rect.x + self->rect.x - 18.0f;
    iconRect.y = rect.y + self->rect.y - 18.0f;
    iconRect.width = 36.0f;
    iconRect.height = 36.0f;
    SpritesheetDraw(ssIcons, iw->frame, 1.0f, iconRect);
}
