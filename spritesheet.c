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
    struct SpriteInfo* info;
    int maxFrame;
    Vector2 frameSize;
    int framesWide;
    Vector2 origin;
};

struct Spritesheet* SpritesheetNew(const char * fname, int x, int y, int maxFrame, int framesWide) {
    struct Spritesheet* self = malloc(sizeof(struct Spritesheet));
    if (NULL == self) {
        return NULL;
    }
    self->texture = LoadTexture(fname);
    self->info = NULL;
    self->frameSize.x = x;
    self->frameSize.y = y;
    self->maxFrame = maxFrame;
    self->framesWide = framesWide;
    self->origin.x = 0;
    self->origin.y = 0;
    return self;
}

struct Spritesheet* SpritesheetNewInfo(const char* fname, struct SpriteInfo* info, int maxFrame) {
    struct Spritesheet* self = malloc(sizeof(struct Spritesheet));
    if (NULL == self) {
        return NULL;
    }
    self->texture = LoadTexture(fname);
    self->info = info;
    self->maxFrame = maxFrame;
    return self;
}

void SpritesheetFree(struct Spritesheet *self) {
    if (NULL == self) {
        return;
    }
    UnloadTexture(self->texture);
    free(self);
}

void SpritesheetDraw(struct Spritesheet *self, int frame, float x, float y) {
    const float scale = 1.0;
    const float ang = 0.0;
    const Color c = WHITE;
    if ( self->info == NULL ) {
        float ox = (frame % self->framesWide) * self->frameSize.x;
        float oy = (frame / self->framesWide) * self->frameSize.y;
        DrawTexturePro(
            self->texture,
            (Rectangle){ox, oy, self->frameSize.x,self->frameSize.y}, 
            (Rectangle){x, y, self->frameSize.x * scale, self->frameSize.y * scale}, 
            (Vector2){self->origin.x * scale, self->origin.y * scale},
            ang,
            c
        );
    } else {
        DrawTexturePro(
            self->texture,
            (Rectangle){.x = (float)self->info[frame].x, .y=(float)self->info[frame].y, .width=(float)self->info[frame].w, .height=(float)self->info[frame].h},
            (Rectangle){.x = x, .y = y, .width = (float)self->info[frame].w, .height = (float)self->info[frame].h},
            (Vector2){self->origin.x * scale, self->origin.y * scale},
            ang,
            c
        );
    }
}
