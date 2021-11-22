/* spritesheet.h */

#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <raylib.h>

struct Spritesheet;

struct Spritesheet* SpritesheetNew(const char * fname, float x, float y, int framesWide);
struct Spritesheet* SpritesheetNewImage(Image img, float x, float y, int framesWide);
struct Spritesheet* SpritesheetNewInfo(const char * fname, float x, float y, Vector2 *coords);
void SpritesheetFree(struct Spritesheet *const self);
void SpritesheetDraw(struct Spritesheet *const self, int frame, float xScale, float ang, Rectangle r);

#endif
