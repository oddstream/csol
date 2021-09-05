/* spritesheet.h */

#ifndef SPRITESHEET_H
#define SPRITESHEET_H

struct Spritesheet;

struct SpriteInfo {
    int x, y, w, h;
};

struct Spritesheet* SpritesheetNew(const char * fname, int x, int y, int maxFrame, int framesWide);
struct Spritesheet* SpritesheetNewInfo(const char * fname, struct SpriteInfo* info, int maxFrame);
void SpritesheetFree(struct Spritesheet*);
void SpritesheetDraw(struct Spritesheet *s, int frame, float x, float y);

#endif
