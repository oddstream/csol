/* spritesheet.h */

struct spritesheet;

struct spritesheet* spritesheet_new(const char * fname, int x, int y, int maxFrame, int framesWide);
void spritesheet_dispose(struct spritesheet*);
void spritesheet_draw(struct spritesheet *s, int frame, float x, float y);
