/* iconwidget.c */

#include "command.h"
#include "spritesheet.h"
#include "ui.h"

static struct WidgetVtable iconWidgetVtable = {
    &WidgetUpdate,  // nothing to update, so use super update
    &IconWidgetDraw,
    &WidgetFree,    // nothing extra to free, so use base free
};

struct IconWidget* IconWidgetNew(struct Container *parent, int align, enum IconName frame, CommandFunction cf, void* param)
{
    struct IconWidget* self = calloc(1, sizeof(struct IconWidget));
    if ( self ) {
        WidgetCtor((struct Widget*)self, parent, align, cf, param);
        self->super.vtable = &iconWidgetVtable;
        self->frame = frame;
        ((struct Widget*)self)->rect.width = 36.0f;
        ((struct Widget*)self)->rect.height = 36.0f;
    }
    return self;
}

void IconWidgetDraw(struct Widget *const self)
{
    extern struct Spritesheet *ssIcons;

    struct IconWidget *const iw = (struct IconWidget*)self;

    // get the container's screen rect
    struct Container *const con = self->parent;
    // add our x,y to the container's x,y to get screen position
    Rectangle iconRect;
    iconRect.x = con->rect.x + self->rect.x;
    iconRect.y = con->rect.y + self->rect.y;
    iconRect.width = self->rect.width;
    iconRect.height = self->rect.height;
    if ( self->bcf && CheckCollisionPointRec(GetMousePosition(), iconRect) ) {
        iconRect.x += 2.0f;
        iconRect.y += 2.0f;
    }
    SpritesheetDraw(ssIcons, iw->frame, 1.0f, 0.0f, iconRect);
}
