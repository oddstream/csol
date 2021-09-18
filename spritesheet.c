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
    int maxFrame;
    Vector2 frameSize;
    int framesWide;
};

struct Spritesheet* SpritesheetNew(const char * fname, int x, int y, int maxFrame, int framesWide) {
    struct Spritesheet* self = malloc(sizeof(struct Spritesheet));
    if ( !self ) {
        return NULL;
    }
    self->texture = LoadTexture(fname);
    self->coords = NULL;
    self->frameSize.x = x;
    self->frameSize.y = y;
    self->maxFrame = maxFrame;
    self->framesWide = framesWide;
    return self;
}

struct Spritesheet* SpritesheetNewInfo(const char* fname, Vector2 *coords, int maxFrame) {
    struct Spritesheet* self = malloc(sizeof(struct Spritesheet));
    if ( !self ) {
        return NULL;
    }
    self->texture = LoadTexture(fname);
    self->coords = coords;
    self->maxFrame = maxFrame;
    return self;
}

void SpritesheetFree(struct Spritesheet *const self) {
    if ( !self ) {
        return;
    }
    UnloadTexture(self->texture);
    free(self);
}

void SpritesheetDraw(struct Spritesheet *const self, int frame, float xScale, Vector2 pos) {
    extern float cardWidth, cardHeight;
    const float yScale = 1.0f;
    const float ang = 0.0f;
    const Color c = WHITE;
    if ( self->coords ) {
        // extern float cardWidth, cardHeight;
        // Rectangle r = {.x=x, .y=y, cardWidth, cardHeight};
        // DrawRectangleRounded(r, 0.1, 4, (Color){0,0,127,255});
        Rectangle rSrc = {.x=self->coords[frame].x, .y=self->coords[frame].y, .width=cardWidth, .height=cardHeight};
        // float dstWidth = self->info[frame].width * xScale;
        // float xOffset = (self->info[frame].width / 2.0f) - (dstWidth / 2.0f);
        // Rectangle rDst = (Rectangle){.x = pos.x + xOffset, .y = pos.y, .width = self->info[frame].width * xScale, .height = self->info[frame].height * yScale};
        float dstWidth = cardWidth * xScale;
        float xOffset = (cardWidth / 2.0f) - (dstWidth / 2.0f);
        Rectangle rDst = (Rectangle){.x = pos.x + xOffset, .y = pos.y, .width = cardWidth * xScale, .height = cardHeight * yScale};
        DrawTexturePro(
            self->texture,
            rSrc,
            rDst,
            (Vector2){0},//{self->origin.x * scale, self->origin.y * scale},
            ang,
            c
        );
    } else {
        float ox = (frame % self->framesWide) * self->frameSize.x;
        float oy = (frame / self->framesWide) * self->frameSize.y;
        // TODO tidy up this math
        float dstWidth = self->frameSize.x * xScale;
        // float xOffset = self->frameSize.x - dstWidth / 2.0f;
        // xOffset -= self->frameSize.x / 2.0f;
        float xOffset = (self->frameSize.x / 2.0f) - (dstWidth / 2.0f);
        DrawTexturePro(
            self->texture,
            (Rectangle){ox, oy, self->frameSize.x,self->frameSize.y}, 
            (Rectangle){pos.x + xOffset, pos.y, self->frameSize.x * xScale, self->frameSize.y * yScale}, 
            (Vector2){0},//{self->origin.x * scale, self->origin.y * scale},
            ang,
            c
        );
    }
}
