/* textwidget.c */

#include <string.h>

#include "ui.h"

static struct WidgetVtable textWidgetVtable = {
    &WidgetUpdate,  // nothing to update, so use super update
    &TextWidgetDraw,
    &TextWidgetFree,
};

struct TextWidget* TextWidgetNew(struct Container *parent, Font *font, float fontSize, int align)
{
    struct TextWidget* self = calloc(1, sizeof(struct TextWidget));
    if ( self ) {
        WidgetCtor((struct Widget*)self, parent, align, NULL);
        self->super.vtable = &textWidgetVtable;
        self->font = font;
        self->fontSize = fontSize;
        self->text = NULL;
    }
    return self;
}

void TextWidgetSetText(struct TextWidget *const self, const char* text)
{
    if ( self->text ) {
        free(self->text);
        self->text = NULL;
    }
    if ( text ) {
        self->text = strdup(text);
        Vector2 mte = MeasureTextEx(*(self->font), text, self->fontSize, 1.2f);
        self->super.rect.width = mte.x;
        self->super.rect.height = mte.y;
    }
}

void TextWidgetDraw(struct Widget *const self)
{
    extern Color uiTextColor;

    struct TextWidget *const tw = (struct TextWidget*)self;
    if ( tw->text == NULL ) {
        return;
    }

    // get the container's screen rect
    struct Container *const con = self->parent;
    Rectangle rect = con->rect;
    // add our x,y to the container's x,y to get screen position
    Vector2 pos;
    pos.x = rect.x + self->rect.x;
    pos.y = rect.y + self->rect.y;
    DrawTextEx(*(tw->font), tw->text, pos, tw->fontSize, 1.2f, uiTextColor);
}

void TextWidgetFree(struct Widget *const self)
{
    if ( self ) {
        struct TextWidget *const tw = (struct TextWidget*)self;
        if ( tw->text ) {
            free(tw->text);
        }
        WidgetFree(self);
    }
}