/* drawer.c */

#include <stdio.h>

#include "ui.h"

void DrawerCtor(struct Drawer *const self, Rectangle r)
{
    ContainerCtor(&self->super, r);
    self->aniState = STOP;
}

void DrawerStartDrag(struct Container *const self, Vector2 pos)
{
    fprintf(stdout, "DrawerStartDrag %.0f, %.0f\n", pos.x, pos.y);

    struct Drawer *const dr = (struct Drawer*)self;
    dr->dragStartPos = pos;
}

void DrawerDragBy(struct Container *const self, Vector2 delta)
{
    // delta is the difference between the point now, and what it was previously
    // (in gosol, delta is the difference between the point now and where the drag started)

    struct Drawer *const dr = (struct Drawer*)self;
    // dr->dragOffset.x = dr->dragOffset.x + delta.x;
    // if ( dr->dragOffset.x > 0.0f ) {
    //     dr->dragOffset.x = 0.0f;
    // }
    dr->dragOffset.y = dr->dragOffset.y + delta.y;
    if ( dr->dragOffset.y > 0.0f ) {
        dr->dragOffset.y = 0.0f;
    }

    DrawerLayoutWidgets(self);

    fprintf(stdout, "DrawerDragBy %.0f,%.0f := %.0f,%.0f\n", delta.x, delta.y, dr->dragOffset.x, dr->dragOffset.y);
}

void DrawerStopDrag(struct Container *const self, Vector2 pos)
{
    fprintf(stdout, "DrawerStopDrag %.0f, %.0f\n", pos.x, pos.y);

    (void)self;
    (void)pos;
}

bool DrawerWasDragged(struct Container *const self, Vector2 pos)
{
    struct Drawer *const dr = (struct Drawer*)self;
    bool result = !(pos.x == dr->dragStartPos.x && pos.y == dr->dragStartPos.y);
    fprintf(stdout, "DrawerWasDragged %d\n", result);
    return result;
}

void DrawerLayoutWidgets(struct Container *const self)
{
    // arrange widgets vertically, stacked top to bottom
    // ignore widget align

    float x = WIDGET_PADDING;
    float y = WIDGET_PADDING;
    size_t index;
    struct Drawer *dr = (struct Drawer*)self;
    for ( struct Widget *w = ArrayFirst(dr->super.widgets, &index); w; w = ArrayNext(dr->super.widgets, &index) ) {
        w->rect.x = x + dr->dragOffset.x;
        w->rect.y = y + dr->dragOffset.y;
        y += w->rect.height + WIDGET_PADDING;
    }
}

void DrawerLayout(struct Container *const self, const int windowWidth, const int windowHeight)
{
    (void)windowWidth;

    struct Drawer *dr = (struct Drawer*)self;
    dr->super.rect = (Rectangle){.x=-DRAWER_WIDTH, .y=TITLEBAR_HEIGHT, .width=DRAWER_WIDTH, .height=windowHeight - TITLEBAR_HEIGHT - STATUSBAR_HEIGHT};
    DrawerLayoutWidgets(self);
}

void DrawerUpdate(struct Container *const self)
{
    struct Drawer *dr = (struct Drawer*)self;

    ContainerUpdate(self);  // does nothing?

    switch ( dr->aniState ) {
        case LEFT:
            if ( dr->super.rect.x <= -DRAWER_WIDTH ) {
                dr->super.rect.x = -(DRAWER_WIDTH);
                dr->aniState = STOP;
            } else {
                dr->super.rect.x -= 16.0f;
            }
            break;
        case STOP:
            break;
        case RIGHT:
            if ( dr->super.rect.x >= 0.0f ) {
                dr->super.rect.x = 0.0f;
                dr->aniState = STOP;
            } else {
                dr->super.rect.x += 16.0f;
            }
            break;
        default:
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
    // return self->super.rect.x > -1.0f;   // don't compare a float with 0
    return self->super.rect.x > -DRAWER_WIDTH / 2.0f;
}

void DrawerShow(struct Drawer *const self)
{
    self->aniState = RIGHT;
    // reset any drag offset so list starts un-vscrolled
    self->dragOffset = (Vector2){.x=0.0f, .y=0.0f};
    DrawerLayoutWidgets(&(self->super));
}

void DrawerHide(struct Drawer *const self)
{
    if ( self->super.rect.x == -DRAWER_WIDTH ) {
        self->aniState = STOP;
    } else {
        self->aniState = LEFT;
    }
}