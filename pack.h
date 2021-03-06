/* pack.h */

#ifndef PACK_H
#define PACK_H

#include <raylib.h>

#include "spritesheet.h"

struct Pack {
    char *name;
    char *description;
    float roundness;
    float width, height;

    Vector2 *ssFaceMap;    // always 52 entries
    char *ssFaceFname;
    Vector2 *ssBackMap;
    int backMapEntries;
    char *ssBackFname;
    int backFrame;          // TODO make this a user option

    struct Spritesheet *ssFace;    // built in Ctor
    struct Spritesheet *ssBack;    // built in Ctor

    Font unicodeFont;      // for unicode cards, built in Ctor
    char *unicodeFontFname;
    float unicodeFontExpansion;
    int numberOfColors;

    // piggy-back some Pile things that depend on Card Pack
    Font pileFont;
    int pileFontSize;
    int labelFontSize;
    Font symbolFont;
};

struct Pack *PackCtor(const char *name);
void PackDtor(struct Pack *self);

#endif
