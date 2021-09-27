/* textwidget.c */

#include <string.h>

#include "ui.h"

static struct WidgetVtable textWidgetVtable = {
    &WidgetParent,  // use super parent
    &WidgetRect,    // use super rect
    &WidgetSetPosition, // use super set position
    &WidgetAlign,   // use super align
    &WidgetUpdate,  // nothing to update, so use super update
    &TextWidgetDraw,
    &TextWidgetFree,
};

struct TextWidget* TextWidgetNew(struct Container *parent, int align)
{
    struct TextWidget* self = calloc(1, sizeof(struct TextWidget));
    if ( self ) {
        WidgetCtor((struct Widget*)self, parent, align);
        self->super.vtable = &textWidgetVtable;
        self->text = NULL;
    }
    return self;
}

void TextWidgetSetText(struct TextWidget *const self, const char* text)
{
    extern Font fontRoboto14;

    if ( self->text ) {
        free(self->text);
        self->text = NULL;
    }
    if ( text ) {
        self->text = strdup(text);
        Vector2 mte = MeasureTextEx(fontRoboto14, text, 14.0f, 1.2f);
        self->super.rect.width = mte.x;
        self->super.rect.height = mte.y;
    }
}

void TextWidgetDraw(struct Widget *const self)
{
    extern Font fontRoboto14;
    extern Color uiTextColor;

    struct TextWidget *tw = (struct TextWidget*)self;
    if ( tw->text == NULL ) {
        return;
    }

    Rectangle rect = ContainerRect(self->parent);   // get the container's screen rect
    Vector2 pos;
    pos.x = rect.x + self->rect.x;
    pos.y = rect.y + self->rect.y;
    DrawTextEx(fontRoboto14, tw->text, pos, 14.0f, 1.2f, uiTextColor);
}

void TextWidgetFree(struct Widget *const self)
{
    if ( self ) {
        struct TextWidget *tw = (struct TextWidget*)self;
        if ( tw->text ) {
            free(tw->text);
        }
        WidgetFree(self);
    }
}