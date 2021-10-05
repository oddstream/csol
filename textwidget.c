/* textwidget.c */

#include <string.h>

#include "spritesheet.h"
#include "ui.h"

static struct WidgetVtable textWidgetVtable = {
    &WidgetUpdate,  // nothing to update, so use super update
    &TextWidgetDraw,
    &TextWidgetFree,
};

struct TextWidget* TextWidgetNew(struct Container *parent, enum IconName frame, Font *font, float fontSize, int align, BaizeCommandFunction bcf)
{
    struct TextWidget* self = calloc(1, sizeof(struct TextWidget));
    if ( self ) {
        WidgetCtor((struct Widget*)self, parent, align, bcf);
        self->super.vtable = &textWidgetVtable;
        self->frame = frame;
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
        if ( self->frame == NONE ) {
            self->super.rect.width = mte.x;
            self->super.rect.height = mte.y;
        } else {
            self->super.rect.width = ICON_SIZE + mte.x;
            self->super.rect.height = ICON_SIZE + mte.y;
        }
    }
}

void TextWidgetDraw(struct Widget *const self)
{
    extern Color uiTextColor;

    struct TextWidget *const tw = (struct TextWidget*)self;

    // get the container's screen rect
    struct Container *const con = self->parent;
    Rectangle rect = con->rect;
    // add our x,y to the container's x,y to get screen position
    Vector2 pos;
    pos.x = rect.x + self->rect.x;
    pos.y = rect.y + self->rect.y;
    if ( tw->frame != NONE ) {
        extern struct Spritesheet *ssIcons;
        Rectangle iconRect;
        iconRect.x = con->rect.x + self->rect.x;
        iconRect.y = con->rect.y + self->rect.y;
        iconRect.width = ICON_SIZE;
        iconRect.height = ICON_SIZE;
        SpritesheetDraw(ssIcons, tw->frame, 1.0f, iconRect);

        pos.x += ICON_SIZE;
    }
    if ( tw->text ) {
        // Rectangle screenRect = (Rectangle){.x=pos.x, .y=pos.y, .width=tw->super.rect.width, .height=tw->super.rect.height};
        // if ( CheckCollisionPointRec(GetMousePosition(), screenRect) ) {
        //     pos.x += 2.0f;
        //     pos.y += 2.0f;
        // }
        DrawTextEx(*(tw->font), tw->text, pos, tw->fontSize, 1.2f, uiTextColor);
    }
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