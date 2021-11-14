/* pack.c */

#include <string.h>

#include "pack.h"
#include "trace.h"

static struct Vector2 retroFaceMap[52] = {
    // Club
    {.x=72.0f*0.0f, .y=0},
    {.x=72.0f*1.0f, .y=0},
    {.x=72.0f*2.0f, .y=0},
    {.x=72.0f*3.0f, .y=0},
    {.x=72.0f*4.0f, .y=0},
    {.x=72.0f*5.0f, .y=0},
    {.x=72.0f*6.0f, .y=0},
    {.x=72.0f*7.0f, .y=0},
    {.x=72.0f*8.0f, .y=0},
    {.x=72.0f*9.0f, .y=0},
    {.x=72.0f*10.0f, .y=0},
    {.x=72.0f*11.0f, .y=0},
    {.x=72.0f*12.0f, .y=0},
    // Diamond
    {.x=72.0f*0.0f, .y=96.0f*3.0f},
    {.x=72.0f*1.0f, .y=96.0f*3.0f},
    {.x=72.0f*2.0f, .y=96.0f*3.0f},
    {.x=72.0f*3.0f, .y=96.0f*3.0f},
    {.x=72.0f*4.0f, .y=96.0f*3.0f},
    {.x=72.0f*5.0f, .y=96.0f*3.0f},
    {.x=72.0f*6.0f, .y=96.0f*3.0f},
    {.x=72.0f*7.0f, .y=96.0f*3.0f},
    {.x=72.0f*8.0f, .y=96.0f*3.0f},
    {.x=72.0f*9.0f, .y=96.0f*3.0f},
    {.x=72.0f*10.0f, .y=96.0f*3.0f},
    {.x=72.0f*11.0f, .y=96.0f*3.0f},
    {.x=72.0f*12.0f, .y=96.0f*3.0f},
    // Heart
    {.x=72.0f*0.0f, .y=96.0f*2.0f},
    {.x=72.0f*1.0f, .y=96.0f*2.0f},
    {.x=72.0f*2.0f, .y=96.0f*2.0f},
    {.x=72.0f*3.0f, .y=96.0f*2.0f},
    {.x=72.0f*4.0f, .y=96.0f*2.0f},
    {.x=72.0f*5.0f, .y=96.0f*2.0f},
    {.x=72.0f*6.0f, .y=96.0f*2.0f},
    {.x=72.0f*7.0f, .y=96.0f*2.0f},
    {.x=72.0f*8.0f, .y=96.0f*2.0f},
    {.x=72.0f*9.0f, .y=96.0f*2.0f},
    {.x=72.0f*10.0f, .y=96.0f*2.0f},
    {.x=72.0f*11.0f, .y=96.0f*2.0f},
    {.x=72.0f*12.0f, .y=96.0f*2.0f},
    // Spade
    {.x=72.0f*0.0f, .y=96.0f},
    {.x=72.0f*1.0f, .y=96.0f},
    {.x=72.0f*2.0f, .y=96.0f},
    {.x=72.0f*3.0f, .y=96.0f},
    {.x=72.0f*4.0f, .y=96.0f},
    {.x=72.0f*5.0f, .y=96.0f},
    {.x=72.0f*6.0f, .y=96.0f},
    {.x=72.0f*7.0f, .y=96.0f},
    {.x=72.0f*8.0f, .y=96.0f},
    {.x=72.0f*9.0f, .y=96.0f},
    {.x=72.0f*10.0f, .y=96.0f},
    {.x=72.0f*11.0f, .y=96.0f},
    {.x=72.0f*12.0f, .y=96.0f},
};

static struct Vector2 retroBackMap[13] = {
    {.x = 5, .y = 4},       // Aquarium
    {.x = 85, .y = 4},      // CardHand
    {.x = 165, .y = 4},     // Castle
    {.x = 245, .y = 4},     // Empty / JazzCup
    {.x = 325, .y = 4},     // Fishes
    {.x = 405, .y = 4},     // FlowerBlack
    {.x = 485, .y = 4},     // FlowerBlue
    {.x = 5, .y = 140},     // PalmBeach
    {.x = 85, .y = 140},    // Pattern1
    {.x = 165, .y = 140},   // Pattern2
    {.x = 245, .y = 140},   // Robot
    {.x = 325, .y = 140},   // Roses
    {.x = 405, .y = 140},   // Shell
};

static struct Vector2 kenneyFaceMap[52] = {
    // Club
    {.x=560, .y=570},  // Ace
    {.x=280, .y=1140}, // 2
    {.x=700, .y=190},  // 3
    {.x=700, .y=0},    // 4
    {.x=560, .y=1710}, // 5
    {.x=560, .y=1520}, // 6
    {.x=560, .y=1330}, // 7
    {.x=560, .y=1140}, // 8
    {.x=560, .y=950},  // 9
    {.x=560, .y=760},  // 10
    {.x=560, .y=380},  // J
    {.x=560, .y=0},    // Q
    {.x=560, .y=190},  // K
    // Diamond
    {.x=420, .y=0},    // Ace
    {.x=420, .y=1710}, // 2
    {.x=420, .y=1520}, // 3
    {.x=420, .y=1330}, // 4
    {.x=420, .y=1140}, // 5
    {.x=420, .y=950},  // 6
    {.x=420, .y=760},  // 7
    {.x=420, .y=570},  // 8
    {.x=420, .y=380},  // 9
    {.x=420, .y=190},  // 10
    {.x=280, .y=1710}, // J
    {.x=280, .y=1330}, // Q
    {.x=280, .y=1520}, // K
    // Heart
    {.x=140, .y=1330}, // Ace
    {.x=700, .y=380},  // 2
    {.x=280, .y=950},  // 3
    {.x=280, .y=760},  // 4
    {.x=280, .y=570},  // 5
    {.x=280, .y=380},  // 6
    {.x=280, .y=190},  // 7
    {.x=280, .y=0},    // 8
    {.x=140, .y=1710}, // 9
    {.x=140, .y=1520}, // 10
    {.x=140, .y=1140}, // J
    {.x=140, .y=760},  // Q
    {.x=140, .y=950},  // K
    // Spade
    {.x=0, .y=570},    // Ace
    {.x=140, .y=380},  // 2
    {.x=140, .y=190},  // 3
    {.x=140, .y=0},    // 4
    {.x=0, .y=1710},   // 5
    {.x=0, .y=1520},   // 6
    {.x=0, .y=1330},   // 7
    {.x=0, .y=1140},   // 8
    {.x=0, .y=950},    // 9
    {.x=0, .y=760},    // 10
    {.x=0, .y=380},    // J
    {.x=0, .y=00},     // Q
    {.x=0, .y=190},    // K
};

static struct Vector2 kenneyBackMap[15] = {
    {.x=0, .y=0},
    {.x=140, .y=0},
    {.x=280, .y=0},

    {.x=0, .y=190},
    {.x=140, .y=190},
    {.x=280, .y=190},

    {.x=0, .y=380},
    {.x=140, .y=380},
    {.x=280, .y=380},

    {.x=0, .y=570},
    {.x=140, .y=570},
    {.x=280, .y=570},

    {.x=0, .y=760},
    {.x=140, .y=760},
    {.x=280, .y=760},
};

static struct Vector2 simple140x190FaceMap[52] = {
    // Club
    {.x=1*140-140, .y=1*190-190},     // Ace
    {.x=2*140-140, .y=1*190-190},     // 2
    {.x=3*140-140, .y=1*190-190},     // 3
    {.x=4*140-140, .y=1*190-190},     // 4
    {.x=5*140-140, .y=1*190-190},     // 5
    {.x=6*140-140, .y=1*190-190},     // 6
    {.x=7*140-140, .y=1*190-190},     // 7
    {.x=8*140-140, .y=1*190-190},     // 8
    {.x=1*140-140, .y=2*190-190},     // 9
    {.x=2*140-140, .y=2*190-190},     // 10
    {.x=3*140-140, .y=2*190-190},     // J
    {.x=4*140-140, .y=2*190-190},     // Q
    {.x=5*140-140, .y=2*190-190},     // K
    // Diamond
    {.x=6*140-140, .y=2*190-190},     // Ace
    {.x=7*140-140, .y=2*190-190},     // 2
    {.x=8*140-140, .y=2*190-190},     // 3
    {.x=1*140-140, .y=3*190-190},     // 4
    {.x=2*140-140, .y=3*190-190},     // 5
    {.x=3*140-140, .y=3*190-190},     // 6
    {.x=4*140-140, .y=3*190-190},     // 7
    {.x=5*140-140, .y=3*190-190},     // 8
    {.x=6*140-140, .y=3*190-190},     // 9
    {.x=7*140-140, .y=3*190-190},     // 10
    {.x=8*140-140, .y=3*190-190},     // J
    {.x=1*140-140, .y=4*190-190},     // Q
    {.x=2*140-140, .y=4*190-190},     // K
    // Heart
    {.x=3*140-140, .y=4*190-190},     // Ace
    {.x=4*140-140, .y=4*190-190},     // 2
    {.x=5*140-140, .y=4*190-190},     // 3
    {.x=6*140-140, .y=4*190-190},     // 4
    {.x=7*140-140, .y=4*190-190},     // 5
    {.x=8*140-140, .y=4*190-190},     // 6
    {.x=1*140-140, .y=5*190-190},     // 7
    {.x=2*140-140, .y=5*190-190},     // 8
    {.x=3*140-140, .y=5*190-190},     // 9
    {.x=4*140-140, .y=5*190-190},     // 10
    {.x=5*140-140, .y=5*190-190},     // J
    {.x=6*140-140, .y=5*190-190},     // Q
    {.x=7*140-140, .y=5*190-190},     // K
    // Spade
    {.x=3*140-140, .y=6*190-190},     // Ace
    {.x=4*140-140, .y=6*190-190},     // 2
    {.x=5*140-140, .y=6*190-190},     // 3
    {.x=6*140-140, .y=6*190-190},     // 4
    {.x=7*140-140, .y=6*190-190},     // 5
    {.x=8*140-140, .y=6*190-190},     // 6
    {.x=9*140-140, .y=1*190-190},     // 7
    {.x=9*140-140, .y=2*190-190},     // 8
    {.x=9*140-140, .y=3*190-190},     // 9
    {.x=9*140-140, .y=4*190-190},     // 10
    {.x=9*140-140, .y=5*190-190},     // J
    {.x=9*140-140, .y=6*190-190},     // Q
    {.x=1*140-140, .y=7*190-190},     // K
};

static struct Vector2 simple93x127FaceMap[52] = {
    // Club
    {.x=1*93-93, .y=1*127-127},     // Ace
    {.x=2*93-93, .y=1*127-127},     // 2
    {.x=3*93-93, .y=1*127-127},     // 3
    {.x=4*93-93, .y=1*127-127},     // 4
    {.x=5*93-93, .y=1*127-127},     // 5
    {.x=6*93-93, .y=1*127-127},     // 6
    {.x=7*93-93, .y=1*127-127},     // 7
    {.x=8*93-93, .y=1*127-127},     // 8
    {.x=1*93-93, .y=2*127-127},     // 9
    {.x=2*93-93, .y=2*127-127},     // 10
    {.x=3*93-93, .y=2*127-127},     // J
    {.x=4*93-93, .y=2*127-127},     // Q
    {.x=5*93-93, .y=2*127-127},     // K
    // Diamond
    {.x=6*93-93, .y=2*127-127},     // Ace
    {.x=7*93-93, .y=2*127-127},     // 2
    {.x=8*93-93, .y=2*127-127},     // 3
    {.x=1*93-93, .y=3*127-127},     // 4
    {.x=2*93-93, .y=3*127-127},     // 5
    {.x=3*93-93, .y=3*127-127},     // 6
    {.x=4*93-93, .y=3*127-127},     // 7
    {.x=5*93-93, .y=3*127-127},     // 8
    {.x=6*93-93, .y=3*127-127},     // 9
    {.x=7*93-93, .y=3*127-127},     // 10
    {.x=8*93-93, .y=3*127-127},     // J
    {.x=1*93-93, .y=4*127-127},     // Q
    {.x=2*93-93, .y=4*127-127},     // K
    // Heart
    {.x=3*93-93, .y=4*127-127},     // Ace
    {.x=4*93-93, .y=4*127-127},     // 2
    {.x=5*93-93, .y=4*127-127},     // 3
    {.x=6*93-93, .y=4*127-127},     // 4
    {.x=7*93-93, .y=4*127-127},     // 5
    {.x=8*93-93, .y=4*127-127},     // 6
    {.x=1*93-93, .y=5*127-127},     // 7
    {.x=2*93-93, .y=5*127-127},     // 8
    {.x=3*93-93, .y=5*127-127},     // 9
    {.x=4*93-93, .y=5*127-127},     // 10
    {.x=5*93-93, .y=5*127-127},     // J
    {.x=6*93-93, .y=5*127-127},     // Q
    {.x=7*93-93, .y=5*127-127},     // K
    // Spade
    {.x=3*93-93, .y=6*127-127},     // Ace
    {.x=4*93-93, .y=6*127-127},     // 2
    {.x=5*93-93, .y=6*127-127},     // 3
    {.x=6*93-93, .y=6*127-127},     // 4
    {.x=7*93-93, .y=6*127-127},     // 5
    {.x=8*93-93, .y=6*127-127},     // 6
    {.x=9*93-93, .y=1*127-127},     // 7
    {.x=9*93-93, .y=2*127-127},     // 8
    {.x=9*93-93, .y=3*127-127},     // 9
    {.x=9*93-93, .y=4*127-127},     // 10
    {.x=9*93-93, .y=5*127-127},     // J
    {.x=9*93-93, .y=6*127-127},     // Q
    {.x=1*93-93, .y=7*127-127},     // K
};

static struct Vector2 simple93x127BackMap[3] = {
    {.x=1*93-93, .y=1*127-127},
    {.x=2*93-93, .y=1*127-127},
    {.x=3*93-93, .y=1*127-127},
};

static struct Pack Packs[] = {
    /*
        designated initializers:
        omitted members are initialized as zero: 
        "Omitted field members are implicitly initialized the same as objects that have static storage duration."
        (https://gcc.gnu.org/onlinedocs/gcc/Designated-Inits.html)
    */
    {
        .name = "retro",
        .fixed = 1,
        .width = 71.0f,
        .height = 96.0f,
        .roundness = 0.05f,
        .ssFaceMap = retroFaceMap,
        .ssBackMap = retroBackMap,
        .backMapEntries = 13,
        .backFrame = 6,
        .ssFaceFname = "assets/lessblockycards71x96.png",
        .ssBackFname = "assets/windows_16bit_cards.png",
    },
    {
        .name = "kenney",
        .fixed = 1,
        .width = 140.0f,
        .height = 190.0f,
        .roundness = 0.05f, 
        .ssFaceMap = kenneyFaceMap,
        .ssBackMap = kenneyBackMap,
        .backMapEntries = 15,
        .backFrame = 2,
        .ssFaceFname = "assets/playingCards.png",
        .ssBackFname = "assets/playingCardBacks.png",
    },
    {
        .name = "simple140x190",
        .fixed = 1,
        .width = 140.0f,
        .height = 190.0f,
        .roundness = 0.275f, 
        .ssFaceMap = simple140x190FaceMap,
        .ssBackMap = simple93x127BackMap,
        .backMapEntries = 3,
        .backFrame = 0,
        .ssFaceFname = "assets/spritesheet simple 140x190.png",
        .ssBackFname = "assets/spritesheet simple back 93x127.png",
    },
    {
        .name = "simple93x127",
        .fixed = 1,
        .width = 93.0f,
        .height = 127.0f,
        .roundness = 0.275f, 
        .ssFaceMap = simple93x127FaceMap,
        .ssBackMap = simple93x127BackMap,
        .backMapEntries = 3,
        .backFrame = 0,
        .ssFaceFname = "assets/spritesheet simple 93x127.png",
        .ssBackFname = "assets/spritesheet simple back 93x127.png",
    },
    {
        .name = "unicode",
        .fixed = 0,
        .width = 93.0f,
        .height = 127.0f,
        .roundness = 0.25f,

        .fontFname = "assets/DejaVuSans.ttf",
        .fontExpansion = 1.3f,
    }
};

struct Pack *PackCtor(const char *name)
{
    struct Pack *self = (void*)0;
    for ( size_t i=0; i<sizeof(Packs)/sizeof(struct Pack); i++ ) {
        if (strcmp(Packs[i].name, name) == 0) {
            self = &Packs[i];
            break;
        }
    }
    if (!self) {
        CSOL_ERROR("Cannot find pack '%s'", name);
        return (void*)0;
    }
    if (self->ssFaceMap && self->ssFaceFname[0]) {
        self->ssFace = SpritesheetNewInfo(self->ssFaceFname, self->width, self->height, self->ssFaceMap);
    }
    if (self->ssBackMap && self->ssBackFname[0]) {
        self->ssBack = SpritesheetNewInfo(self->ssBackFname, self->width, self->height, self->ssBackMap);
    }
    if (self->fontFname) {
        int cardFontCodePoints[1+14*4]; // beware of Knights
        int i, *p = cardFontCodePoints;
            
        *p++ = 0x1F0A0;   // Playing Card Back
        for ( i=0x1f0a1; i<0x1f0af; i++ ) // Spades
            *p++ = i;
        for ( i=0x1f0b1; i<0x1f0bf; i++ ) // Hearts
            *p++ = i;
        for ( i=0x1f0c1; i<0x1f0cf; i++ ) // Diamonds
            *p++ = i;
        for ( i=0x1f0d1; i<0x1f0df; i++ ) // Clubs
            *p++ = i;
        self->font = LoadFontEx(self->fontFname, self->width * self->fontExpansion, cardFontCodePoints, 1+14*4);
    }

    self->pileFontSize = self->height / 2.0f;
    self->fontAcmePile = LoadFontEx("assets/Acme-Regular.ttf", self->pileFontSize, 0, 0);
    self->labelFontSize = self->height / 6.0f;
    self->fontAcmeLabel = LoadFontEx("assets/Acme-Regular.ttf", self->labelFontSize, 0, 0);

    // https://graphemica.com/search?q=home
    int symbolFontCodePoints[5] = {
        0x2663, /* black club */
        0x2666, /* black diamond */
        0x2665, /* black heart */
        0x2660, /* black spade */
        0x267b, /* recycle */ 
    };
    self->fontSymbol = LoadFontEx("assets/DejaVuSans.ttf", self->pileFontSize, symbolFontCodePoints, 5);

    return self;
}

void PackDtor(struct Pack *self)
{
    if (self->ssFace) {
        SpritesheetFree(self->ssFace);
    }
    if (self->ssBack) {
        SpritesheetFree(self->ssBack);
    }
    if (self->font.baseSize) {
        UnloadFont(self->font);
    }

    UnloadFont(self->fontAcmePile);
    UnloadFont(self->fontAcmeLabel);
    UnloadFont(self->fontSymbol);

}
