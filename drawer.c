/* drawer.c */

#include "ui.h"

void DrawerCtor(struct Drawer *const self, Rectangle r)
{
    ContainerCtor(&self->super, r);
    self->aniState = STOP;
}

void DrawerLayoutWidgets(struct Container *const self)
{
    // arrange widgets vertically, stacked top to bottom
    // ignore widget align
    // TODO add Drawer vscoll offset

    float x = WIDGET_PADDING;
    float y = WIDGET_PADDING;
    size_t index;
    struct Drawer *d = (struct Drawer*)self;
    for ( struct Widget *w = ArrayFirst(d->super.widgets, &index); w; w = ArrayNext(d->super.widgets, &index) ) {
        w->rect.x = x;
        w->rect.y = y;
        y += w->rect.height + WIDGET_PADDING;
    }
}

void DrawerLayout(struct Container *const self, const int windowWidth, const int windowHeight)
{
    (void)windowWidth;

    struct Drawer *d = (struct Drawer*)self;
    d->super.rect = (Rectangle){.x=-DRAWER_WIDTH, .y=TITLEBAR_HEIGHT, .width=DRAWER_WIDTH, .height=windowHeight - TITLEBAR_HEIGHT - STATUSBAR_HEIGHT};
    DrawerLayoutWidgets(self);
}

void DrawerUpdate(struct Container *const self)
{
    struct Drawer *d = (struct Drawer*)self;

    ContainerUpdate(self);  // does nothing?

    switch ( d->aniState ) {
        case LEFT:
            if ( d->super.rect.x <= -DRAWER_WIDTH ) {
                d->super.rect.x = -(DRAWER_WIDTH);
                d->aniState = STOP;
            } else {
                d->super.rect.x -= 16.0f;
            }
            break;
        case STOP:
            break;
        case RIGHT:
            if ( d->super.rect.x >= 0.0f ) {
                d->super.rect.x = 0.0f;
                d->aniState = STOP;
            } else {
                d->super.rect.x += 16.0f;
            }
            break;
    }
}

void DrawerDraw(struct Container *const self)
{
    ContainerDraw(self);    // draws background then widgets
}

void DrawerFree(struct Container *const self)
{
    // no extra variables added to Drawer (only functions), so use base class free
    ContainerFree(self);
}

bool DrawerVisible(struct Drawer *const self)
{
    return self->super.rect.x > -1.0f;   // don't compare a float with 0
}

void DrawerShow(struct Drawer *const self)
{
    self->aniState = RIGHT;
}

void DrawerHide(struct Drawer *const self)
{
    if ( self->super.rect.x == -DRAWER_WIDTH ) {
        self->aniState = STOP;
    } else {
        self->aniState = LEFT;
    }
}