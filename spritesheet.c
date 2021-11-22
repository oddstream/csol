/* spritesheet.c */

#include <stdlib.h>

#include "raylib.h"
#include "spritesheet.h"
#include "card.h"

/*
    "Aquarium":    {X: 5, Y: 4},
    "CardHand":    {X: 85, Y: 4},
    "Castle":      {X: 165, Y: 4},
    "JazzCup":     {X: 245, Y: 4},
    "Fishes":      {X: 325, Y: 4},
    "FlowerBlack": {X: 405, Y: 4},
    "FlowerBlue":  {X: 485, Y: 4},
    "PalmBeach":   {X: 5, Y: 140},
    "PatternOne":  {X: 85, Y: 140},
    "PatternTwo":  {X: 165, Y: 140},
    "Robot":       {X: 245, Y: 140},
    "Roses":       {X: 325, Y: 140},
    "Shell":       {X: 405, Y: 140},
*/

struct Spritesheet {
    Texture2D texture;
    Vector2 *coords;
    Vector2 frameSize;
    int framesWide;
};

struct Spritesheet* SpritesheetNew(const char * fname, float x, float y, int framesWide)
{
    struct Spritesheet* self = calloc(1, sizeof(struct Spritesheet));
    if ( !self ) {
        return NULL;
    }
    self->texture = LoadTexture(fname);
    self->coords = NULL;
    self->frameSize.x = x;
    self->frameSize.y = y;
    self->framesWide = framesWide;
    return self;
}

struct Spritesheet* SpritesheetNewImage(Image img, float x, float y, int framesWide)
{
    struct Spritesheet* self = calloc(1, sizeof(struct Spritesheet));
    if ( !self ) {
        return NULL;
    }
    self->texture = LoadTextureFromImage(img);
    self->coords = NULL;
    self->frameSize.x = x;
    self->frameSize.y = y;
    self->framesWide = framesWide;
    return self;
}

struct Spritesheet* SpritesheetNewInfo(const char* fname, float x, float y, Vector2 *coords)
{
    struct Spritesheet* self = calloc(1, sizeof(struct Spritesheet));
    if (!self) {
        return NULL;
    }
    self->texture = LoadTexture(fname);
    self->frameSize.x = x;
    self->frameSize.y = y;
    self->coords = coords;
    return self;
}

void SpritesheetFree(struct Spritesheet *const self)
{
    if (!self) {
        return;
    }
    UnloadTexture(self->texture);
    free(self);
}

void SpritesheetDraw(struct Spritesheet *const self, int frame, float xScale, float ang, Rectangle r)
{
    Rectangle rSrc, rDst;
    const float yScale = 1.0f;
    const Color c = WHITE;

    if (!self) {
        // can happen while changing pack
        return;
    }

    if (self->coords) {
        rSrc = (Rectangle){.x=self->coords[frame].x, .y=self->coords[frame].y, .width=self->frameSize.x, .height=self->frameSize.y};
        // card dimensions are already scaled
    } else {
        float sx = (frame % self->framesWide) * self->frameSize.x;
        float sy = (frame / self->framesWide) * self->frameSize.y;
        rSrc = (Rectangle){.x=sx, .y=sy, .width=self->frameSize.x, .height=self->frameSize.y};
    }
    float dstWidth = r.width * xScale;
    float dstHeight = r.height * yScale;
    float xOffset = (r.width / 2.0f) - (dstWidth / 2.0f);
    rDst = (Rectangle){.x = r.x + xOffset, .y = r.y, .width = dstWidth, .height = dstHeight};
    DrawTexturePro(
        self->texture,
        rSrc,
        rDst,
        (Vector2){0},//{self->origin.x * scale, self->origin.y * scale},
        ang,
        c
    );
    // Rectangle r = {.x=x, .y=y, cardWidth, cardHeight};
    // DrawRectangleRounded(r, 0.1, 4, (Color){0,0,127,255});
}
