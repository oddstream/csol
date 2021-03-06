/* textwidget.c */

#include <string.h>

#include "command.h"
#include "spritesheet.h"
#include "util.h"
#include "ui.h"

static struct WidgetVtable textWidgetVtable = {
    &WidgetUpdate,  // nothing to update, so use super update
    &TextWidgetDraw,
    &TextWidgetFree,
};

struct TextWidget* TextWidgetNew(struct Container *parent, enum IconName frame, Font *font, float fontSize, int align, CommandFunction cf, void* param)
{
    struct TextWidget* self = calloc(1, sizeof(struct TextWidget));
    if (self) {
        WidgetCtor((struct Widget*)self, parent, align, cf, param);
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
    extern float fontSpacing;

    if (self->text) {
        free(self->text);
        self->text = NULL;
    }
    if (text) {
        self->text = UtilStrDup(text);
        Vector2 mte = MeasureTextEx(*(self->font), self->text, self->fontSize, fontSpacing);
        if (self->frame == NONE) {
            self->super.rect.width = mte.x;
            self->super.rect.height = mte.y;
        } else {
            self->super.rect.width = ICON_SIZE + WIDGET_PADDING + mte.x;
            self->super.rect.height = ICON_SIZE;
        }
    }
    // the size of this widget has changed, so it needs to be repositioned in it's parent container
    struct Widget *w = (struct Widget*)self;
    w->parent->vtable->Layout(w->parent, GetScreenWidth(), GetScreenHeight());
}

void TextWidgetDraw(struct Widget *const self)
{
    extern float fontSpacing;
    extern Color uiTextColor;

    struct TextWidget *const tw = (struct TextWidget*)self;

    // get the container's screen rect
    struct Container *const con = self->parent;
    // add our x,y to the container's x,y to get screen position
    Vector2 pos;
    pos.x = con->rect.x + self->rect.x;
    pos.y = con->rect.y + self->rect.y;

    // draw the icon, if there is one
    if ( tw->frame != NONE ) {
        extern struct Spritesheet *ssIcons;
        Rectangle rectIcon = WidgetScreenRect((struct Widget*)tw);
        rectIcon.width = ICON_SIZE;
        rectIcon.height = ICON_SIZE;

        if (!UtilRectangleWithinRectangle(rectIcon, con->rect)) {
#ifdef _DEBUG
            Rectangle r = WidgetScreenRect((struct Widget*)tw);
            DrawRectangleLines(r.x, r.y, r.width, r.height, RED);
#endif
            return;
        }
        if ( self->bcf && CheckCollisionPointRec(GetMousePosition(), WidgetScreenRect(self)) ) {
            rectIcon.x += 2.0f;
            rectIcon.y += 2.0f;
        }
        SpritesheetDraw(ssIcons, tw->frame, 1.0f, 0.0f, rectIcon);
    }

    // draw the text to the right of the icon
    if ( tw->text ) {
        Rectangle rectText = WidgetScreenRect((struct Widget*)tw);

        if ( tw->frame != NONE ) {
            pos.x += ICON_SIZE;
            pos.x += WIDGET_PADDING;
            pos.y += WIDGET_PADDING / 2.0f;//self->parent->rect.y + (ICON_SIZE / 2.0f) - (self->rect.height / 2.0f);
        }

        if (!UtilRectangleWithinRectangle(rectText, con->rect)) {
#ifdef _DEBUG
            Rectangle r = WidgetScreenRect((struct Widget*)tw);
            DrawRectangleLines(r.x, r.y, r.width, r.height, RED);
#endif
            return;
        }

        if ( self->bcf && CheckCollisionPointRec(GetMousePosition(), WidgetScreenRect(self)) ) {
            pos.x += 2.0f;
            pos.y += 2.0f;
        }

        DrawTextEx(*(tw->font), tw->text, pos, tw->fontSize, fontSpacing, uiTextColor);
    }

#if 0
    {
        Rectangle r = WidgetScreenRect((struct Widget*)tw);
        DrawRectangleLines(r.x, r.y, r.width, r.height, RED);
    }
#endif
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