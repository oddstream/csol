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
    struct Spritesheet* s = malloc(sizeof(struct Spritesheet));
    if (NULL == s) {
        return NULL;
    }
    s->texture = LoadTexture(fname);
    s->info = NULL;
    s->frameSize.x = x;
    s->frameSize.y = y;
    s->maxFrame = maxFrame;
    s->framesWide = framesWide;
    s->origin.x = 0;
    s->origin.y = 0;
    return s;
}

struct Spritesheet* SpritesheetNewInfo(const char* fname, struct SpriteInfo* info, int maxFrame) {
    struct Spritesheet* s = malloc(sizeof(struct Spritesheet));
    if (NULL == s) {
        return NULL;
    }
    s->texture = LoadTexture(fname);
    s->info = info;
    s->maxFrame = maxFrame;
    return s;
}

void SpritesheetFree(struct Spritesheet *s) {
    if (NULL == s) {
        return;
    }
    UnloadTexture(s->texture);
    free(s);
}

void SpritesheetDraw(struct Spritesheet *s, int frame, float x, float y) {
    const float scale = 1.0;
    const float ang = 0.0;
    const Color c = WHITE;
    if ( s->info == NULL ) {
        float ox = (frame % s->framesWide) * s->frameSize.x;
        float oy = (frame / s->framesWide) * s->frameSize.y;
        DrawTexturePro(
            s->texture,
            (Rectangle){ox, oy, s->frameSize.x,s->frameSize.y}, 
            (Rectangle){x, y, s->frameSize.x * scale, s->frameSize.y * scale}, 
            (Vector2){s->origin.x * scale, s->origin.y * scale},
            ang,
            c
        );
    } else {
        DrawTexturePro(
            s->texture,
            (Rectangle){.x = (float)s->info[frame].x, .y=(float)s->info[frame].y, .width=(float)s->info[frame].w, .height=(float)s->info[frame].h},
            (Rectangle){.x = x, .y = y, .width = (float)s->info[frame].w, .height = (float)s->info[frame].h},
            (Vector2){s->origin.x * scale, s->origin.y * scale},
            ang,
            c
        );
    }
}
