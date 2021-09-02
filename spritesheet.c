/* spritesheet.c */

#include <stdlib.h>

#include "raylib.h"
#include "spritesheet.h"
#include "card.h"

struct spritesheet {
    Texture2D texture;
    Vector2 frameSize;
    int maxFrame;
    int framesWide;
    Vector2 origin;
};

struct spritesheet* spritesheet_new(const char * fname, int x, int y, int maxFrame, int framesWide) {
    struct spritesheet* s = malloc(sizeof(struct spritesheet));
    if (NULL == s) {
        return NULL;
    }
    s->texture = LoadTexture(fname);
    s->frameSize.x = x;
    s->frameSize.y = y;
    s->maxFrame = maxFrame;
    s->framesWide = framesWide;
    s->origin.x = 0;
    s->origin.y = 0;
    return s;
}

void spritesheet_dispose(struct spritesheet *s) {
    if (NULL == s) {
        return;
    }
    UnloadTexture(s->texture);
    free(s);
}

void spritesheet_draw(struct spritesheet *s, int frame, float x, float y) {
    const float scale = 1.0;
    const float ang = 0.0;
    const Color c = WHITE;
    float ox = (frame % s->framesWide) * s->frameSize.x;
    float oy = (int)(frame / s->framesWide) * s->frameSize.y;
    DrawTexturePro(s->texture, (Rectangle){ox, oy, s->frameSize.x,s->frameSize.y}, 
                    (Rectangle){x, y, s->frameSize.x * scale, s->frameSize.y * scale}, 
                    (Vector2){s->origin.x * scale, s->origin.y * scale}, ang, c);
}
