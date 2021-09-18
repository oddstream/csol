/* spritesheet.h */

#ifndef SPRITESHEET_H
#define SPRITESHEET_H

struct Spritesheet;

struct Spritesheet* SpritesheetNew(const char * fname, int x, int y, int maxFrame, int framesWide);
struct Spritesheet* SpritesheetNewInfo(const char * fname, Vector2 *coords, int maxFrame);
void SpritesheetFree(struct Spritesheet *const self);
void SpritesheetDraw(struct Spritesheet *const self, int frame, float xScale, Vector2 pos);

#endif
