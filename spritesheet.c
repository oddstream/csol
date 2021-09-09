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
    Rectangle* info;
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
    self->info = NULL;
    self->frameSize.x = x;
    self->frameSize.y = y;
    self->maxFrame = maxFrame;
    self->framesWide = framesWide;
    return self;
}

struct Spritesheet* SpritesheetNewInfo(const char* fname, Rectangle* info, int maxFrame) {
    struct Spritesheet* self = malloc(sizeof(struct Spritesheet));
    if ( !self ) {
        return NULL;
    }
    self->texture = LoadTexture(fname);
    self->info = info;
    self->maxFrame = maxFrame;
    return self;
}

void SpritesheetFree(struct Spritesheet* self) {
    if ( !self ) {
        return;
    }
    UnloadTexture(self->texture);
    free(self);
}

void SpritesheetDraw(struct Spritesheet *self, int frame, Vector2 pos) {
    const float scale = 1.0;
    const float ang = 0.0;
    const Color c = WHITE;
    if ( self->info ) {
        // extern float cardWidth, cardHeight;
        // Rectangle r = {.x=x, .y=y, cardWidth, cardHeight};
        // DrawRectangleRounded(r, 0.1, 4, (Color){0,0,127,255});
        Rectangle rSrc = self->info[frame];
        Rectangle rDst = (Rectangle){.x = pos.x, .y = pos.y, .width = self->info[frame].width, .height = self->info[frame].height};
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
        DrawTexturePro(
            self->texture,
            (Rectangle){ox, oy, self->frameSize.x,self->frameSize.y}, 
            (Rectangle){pos.x, pos.y, self->frameSize.x * scale, self->frameSize.y * scale}, 
            (Vector2){0},//{self->origin.x * scale, self->origin.y * scale},
            ang,
            c
        );
    }
}
