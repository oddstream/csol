/* navdrawer.c */

#include "ui.h"

static struct ContainerVtable navDrawerVtable = {
    &DrawerLayoutWidgets,
    &DrawerLayout,
    &DrawerUpdate,
    &DrawerDraw,     // no extra stuff
    &DrawerFree,
};

struct NavDrawer* NavDrawerNew(void)
{
    struct NavDrawer *self = calloc(1, sizeof(struct NavDrawer));
    if ( self ) {
        float h = (float)GetScreenHeight();
        DrawerCtor((struct Drawer*)self, (Rectangle){.x=-DRAWER_WIDTH, .y=TITLEBAR_HEIGHT, .width=DRAWER_WIDTH, .height=h - TITLEBAR_HEIGHT - STATUSBAR_HEIGHT});
        self->super.super.vtable = &navDrawerVtable;
    }
    return self;
}

void NavDrawerFree(struct Container *const self)
{
    DrawerFree(self);
}