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

#if 0
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
#endif

static struct Vector2 kenney96FaceMap[52] = {
    // Club
    {.x= 1*150-150, .y=2*200-200},     // Ace
    {.x= 2*150-150, .y=2*200-200},     // 2
    {.x= 3*150-150, .y=2*200-200},     // 3
    {.x= 4*150-150, .y=2*200-200},     // 4
    {.x= 5*150-150, .y=2*200-200},     // 5
    {.x= 6*150-150, .y=2*200-200},     // 6
    {.x= 7*150-150, .y=2*200-200},     // 7
    {.x= 8*150-150, .y=2*200-200},     // 8
    {.x= 9*150-150, .y=2*200-200},     // 9
    {.x=10*150-150, .y=2*200-200},     // 10
    {.x=11*150-150, .y=2*200-200},     // J
    {.x=12*150-150, .y=2*200-200},     // Q
    {.x=13*150-150, .y=2*200-200},     // K
    // Diamond
    {.x= 1*150-150, .y=3*200-200},     // Ace
    {.x= 2*150-150, .y=3*200-200},     // 2
    {.x= 3*150-150, .y=3*200-200},     // 3
    {.x= 4*150-150, .y=3*200-200},     // 4
    {.x= 5*150-150, .y=3*200-200},     // 5
    {.x= 6*150-150, .y=3*200-200},     // 6
    {.x= 7*150-150, .y=3*200-200},     // 7
    {.x= 8*150-150, .y=3*200-200},     // 8
    {.x= 9*150-150, .y=3*200-200},     // 9
    {.x=10*150-150, .y=3*200-200},     // 10
    {.x=11*150-150, .y=3*200-200},     // J
    {.x=12*150-150, .y=3*200-200},     // Q
    {.x=13*150-150, .y=3*200-200},     // K
    // Heart
    {.x= 1*150-150, .y=4*200-200},     // Ace
    {.x= 2*150-150, .y=4*200-200},     // 2
    {.x= 3*150-150, .y=4*200-200},     // 3
    {.x= 4*150-150, .y=4*200-200},     // 4
    {.x= 5*150-150, .y=4*200-200},     // 5
    {.x= 6*150-150, .y=4*200-200},     // 6
    {.x= 7*150-150, .y=4*200-200},     // 7
    {.x= 8*150-150, .y=4*200-200},     // 8
    {.x= 9*150-150, .y=4*200-200},     // 9
    {.x=10*150-150, .y=4*200-200},     // 10
    {.x=11*150-150, .y=4*200-200},     // J
    {.x=12*150-150, .y=4*200-200},     // Q
    {.x=13*150-150, .y=4*200-200},     // K
    // Spade
    {.x= 1*150-150, .y=0},     // Ace
    {.x= 2*150-150, .y=0},     // 2
    {.x= 3*150-150, .y=0},     // 3
    {.x= 4*150-150, .y=0},     // 4
    {.x= 5*150-150, .y=0},     // 5
    {.x= 6*150-150, .y=0},     // 6
    {.x= 7*150-150, .y=0},     // 7
    {.x= 8*150-150, .y=0},     // 8
    {.x= 9*150-150, .y=0},     // 9
    {.x=10*150-150, .y=0},     // 10
    {.x=11*150-150, .y=0},     // J
    {.x=12*150-150, .y=0},     // Q
    {.x=13*150-150, .y=0},     // K
};

static struct Vector2 kenney96BackMap[15] = {
    {.x=   0, .y=1*200-200},
    {.x= 150, .y=1*200-200},
    {.x= 300, .y=1*200-200},
    {.x= 450, .y=1*200-200},
    {.x= 600, .y=1*200-200},

    {.x=   0, .y=2*200-200},
    {.x= 150, .y=2*200-200},
    {.x= 300, .y=2*200-200},
    {.x= 450, .y=2*200-200},
    {.x= 600, .y=2*200-200},

    {.x=   0, .y=3*200-200},
    {.x= 150, .y=3*200-200},
    {.x= 300, .y=3*200-200},
    {.x= 450, .y=3*200-200},
    {.x= 600, .y=3*200-200},
};

static struct Vector2 kenney64FaceMap[52] = {
    // Club
    {.x=   0, .y=133},     // Ace
    {.x= 100, .y=133},     // 2
    {.x= 200, .y=133},     // 3
    {.x= 300, .y=133},     // 4
    {.x= 400, .y=133},     // 5
    {.x= 500, .y=133},     // 6
    {.x= 600, .y=133},     // 7
    {.x= 700, .y=133},     // 8
    {.x= 800, .y=133},     // 9
    {.x= 900, .y=133},     // 10
    {.x=1000, .y=133},     // J
    {.x=1100, .y=133},     // Q
    {.x=1200, .y=133},     // K
    // Diamond
    {.x=   0, .y=267},     // Ace
    {.x= 100, .y=267},     // 2
    {.x= 200, .y=267},     // 3
    {.x= 300, .y=267},     // 4
    {.x= 400, .y=267},     // 5
    {.x= 500, .y=267},     // 6
    {.x= 600, .y=267},     // 7
    {.x= 700, .y=267},     // 8
    {.x= 800, .y=267},     // 9
    {.x= 900, .y=267},     // 10
    {.x=1000, .y=267},     // J
    {.x=1100, .y=267},     // Q
    {.x=1200, .y=267},     // K
    // Heart
    {.x=   0, .y=400},     // Ace
    {.x= 100, .y=400},     // 2
    {.x= 200, .y=400},     // 3
    {.x= 300, .y=400},     // 4
    {.x= 400, .y=400},     // 5
    {.x= 500, .y=400},     // 6
    {.x= 600, .y=400},     // 7
    {.x= 700, .y=400},     // 8
    {.x= 800, .y=400},     // 9
    {.x= 900, .y=400},     // 10
    {.x=1000, .y=400},     // J
    {.x=1100, .y=400},     // Q
    {.x=1200, .y=400},     // K
    // Spade
    {.x=   0, .y=0},     // Ace
    {.x= 100, .y=0},     // 2
    {.x= 200, .y=0},     // 3
    {.x= 300, .y=0},     // 4
    {.x= 400, .y=0},     // 5
    {.x= 500, .y=0},     // 6
    {.x= 600, .y=0},     // 7
    {.x= 700, .y=0},     // 8
    {.x= 800, .y=0},     // 9
    {.x= 900, .y=0},     // 10
    {.x=1000, .y=0},     // J
    {.x=1100, .y=0},     // Q
    {.x=1200, .y=0},     // K
};

static struct Vector2 kenney64BackMap[15] = {
    {.x=  0, .y=0},
    {.x=100, .y=0},
    {.x=200, .y=0},
    {.x=300, .y=0},
    {.x=400, .y=0},

    {.x=  0, .y=133},
    {.x=100, .y=133},
    {.x=200, .y=133},
    {.x=300, .y=133},
    {.x=400, .y=133},

    {.x=  0, .y=266},
    {.x=100, .y=266},
    {.x=200, .y=266},
    {.x=300, .y=266},
    {.x=400, .y=266},
};

static struct Vector2 kenney48FaceMap[52] = {
    // Club
    {.x= 1*75-75, .y=2*100-100},     // Ace
    {.x= 2*75-75, .y=2*100-100},     // 2
    {.x= 3*75-75, .y=2*100-100},     // 3
    {.x= 4*75-75, .y=2*100-100},     // 4
    {.x= 5*75-75, .y=2*100-100},     // 5
    {.x= 6*75-75, .y=2*100-100},     // 6
    {.x= 7*75-75, .y=2*100-100},     // 7
    {.x= 8*75-75, .y=2*100-100},     // 8
    {.x= 9*75-75, .y=2*100-100},     // 9
    {.x=10*75-75, .y=2*100-100},     // 10
    {.x=11*75-75, .y=2*100-100},     // J
    {.x=12*75-75, .y=2*100-100},     // Q
    {.x=13*75-75, .y=2*100-100},     // K
    // Diamond
    {.x= 1*75-75, .y=3*100-100},     // Ace
    {.x= 2*75-75, .y=3*100-100},     // 2
    {.x= 3*75-75, .y=3*100-100},     // 3
    {.x= 4*75-75, .y=3*100-100},     // 4
    {.x= 5*75-75, .y=3*100-100},     // 5
    {.x= 6*75-75, .y=3*100-100},     // 6
    {.x= 7*75-75, .y=3*100-100},     // 7
    {.x= 8*75-75, .y=3*100-100},     // 8
    {.x= 9*75-75, .y=3*100-100},     // 9
    {.x=10*75-75, .y=3*100-100},     // 10
    {.x=11*75-75, .y=3*100-100},     // J
    {.x=12*75-75, .y=3*100-100},     // Q
    {.x=13*75-75, .y=3*100-100},     // K
    // Heart
    {.x= 1*75-75, .y=4*100-100},     // Ace
    {.x= 2*75-75, .y=4*100-100},     // 2
    {.x= 3*75-75, .y=4*100-100},     // 3
    {.x= 4*75-75, .y=4*100-100},     // 4
    {.x= 5*75-75, .y=4*100-100},     // 5
    {.x= 6*75-75, .y=4*100-100},     // 6
    {.x= 7*75-75, .y=4*100-100},     // 7
    {.x= 8*75-75, .y=4*100-100},     // 8
    {.x= 9*75-75, .y=4*100-100},     // 9
    {.x=10*75-75, .y=4*100-100},     // 10
    {.x=11*75-75, .y=4*100-100},     // J
    {.x=12*75-75, .y=4*100-100},     // Q
    {.x=13*75-75, .y=4*100-100},     // K
    // Spade
    {.x= 1*75-75, .y=1*100-100},     // Ace
    {.x= 2*75-75, .y=1*100-100},     // 2
    {.x= 3*75-75, .y=1*100-100},     // 3
    {.x= 4*75-75, .y=1*100-100},     // 4
    {.x= 5*75-75, .y=1*100-100},     // 5
    {.x= 6*75-75, .y=1*100-100},     // 6
    {.x= 7*75-75, .y=1*100-100},     // 7
    {.x= 8*75-75, .y=1*100-100},     // 8
    {.x= 9*75-75, .y=1*100-100},     // 9
    {.x=10*75-75, .y=1*100-100},     // 10
    {.x=11*75-75, .y=1*100-100},     // J
    {.x=12*75-75, .y=1*100-100},     // Q
    {.x=13*75-75, .y=1*100-100},     // K
};

static struct Vector2 kenney48BackMap[15] = {
    {.x=1*75-75, .y=1*100-100},
    {.x=2*75-75, .y=1*100-100},
    {.x=3*75-75, .y=1*100-100},
    {.x=4*75-75, .y=1*100-100},
    {.x=5*75-75, .y=1*100-100},

    {.x=1*75-75, .y=2*100-100},
    {.x=2*75-75, .y=2*100-100},
    {.x=3*75-75, .y=2*100-100},
    {.x=4*75-75, .y=2*100-100},
    {.x=5*75-75, .y=2*100-100},

    {.x=1*75-75, .y=3*100-100},
    {.x=2*75-75, .y=3*100-100},
    {.x=3*75-75, .y=3*100-100},
    {.x=4*75-75, .y=3*100-100},
    {.x=5*75-75, .y=3*100-100},
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
        .name = "large",
        .fixed = 1,
        .width = 140.0f,
        .height = 190.0f,
        .roundness = 0.05f,
        .ssFaceMap = kenney96FaceMap,
        .ssBackMap = kenney96BackMap,
        .backMapEntries = 15,
        .backFrame = 14,
        .ssFaceFname = "assets/playingCards96.png",
        .ssBackFname = "assets/playingCardBacks96.png",
    },
    {
        .name = "medium",
        .fixed = 1,
        .width = 94.0f,
        .height = 127.0f,
        .roundness = 0.05f,
        .ssFaceMap = kenney64FaceMap,
        .ssBackMap = kenney64BackMap,
        .backMapEntries = 15,
        .backFrame = 14,
        .ssFaceFname = "assets/playingCards64.png",
        .ssBackFname = "assets/playingCardBacks64.png",
    },
    {
        .name = "small",
        .fixed = 1,
        .width = 71.0f,
        .height = 95.0f,
        .roundness = 0.05f,
        .ssFaceMap = kenney48FaceMap,
        .ssBackMap = kenney48BackMap,
        .backMapEntries = 15,
        .backFrame = 14,
        .ssFaceFname = "assets/playingCards48.png",
        .ssBackFname = "assets/playingCardBacks48.png",
    },
#if 0
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
#endif
    {
        .name = "unicode",
        .fixed = 0,
        .width = 93.0f,
        .height = 127.0f,
        .roundness = 0.25f,

        .unicodeFontFname = "assets/DejaVuSans.ttf",
        .unicodeFontExpansion = 1.3f,
        .numberOfColors = 2,
    },
    {
        .name = "fourcolor",
        .fixed = 0,
        .width = 93.0f,
        .height = 127.0f,
        .roundness = 0.25f,

        .unicodeFontFname = "assets/DejaVuSans.ttf",
        .unicodeFontExpansion = 1.3f,
        .numberOfColors = 4,
    }
};

struct Pack *PackCtor(const char *name)
{
    extern int flag_nodraw;

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

    flag_nodraw = 1;

    if (self->ssFaceFname) {
        if (self->ssFaceMap) {
            self->ssFace = SpritesheetNewInfo(self->ssFaceFname, self->width, self->height, self->ssFaceMap);
        } else {
            self->ssFace = SpritesheetNew(self->ssFaceFname, self->width, self->height, 13);
        }
    }
    if (self->ssBackFname) {
        if (self->ssBackMap) {
            self->ssBack = SpritesheetNewInfo(self->ssBackFname, self->width, self->height, self->ssBackMap);
        } else {
            self->ssBack = SpritesheetNew(self->ssBackFname, self->width, self->height, 13);
        }
    }
    if (self->unicodeFontFname) {
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
        self->unicodeFont = LoadFontEx(self->unicodeFontFname, self->width * self->unicodeFontExpansion, cardFontCodePoints, 1+14*4);
    }

    self->pileFont = LoadFontEx("assets/RobotoSlab-Bold.ttf", self->height / 2.0f, 0, 0);
    self->pileFontSize = self->height / 2.0f;
    self->labelFontSize = self->height / 6.0f;

    // https://graphemica.com/search?q=home
    int symbolFontCodePoints[5] = {
        0x2663, /* black club */
        0x2666, /* black diamond */
        0x2665, /* black heart */
        0x2660, /* black spade */
        0x267b, /* recycle */ 
    };
    self->symbolFont = LoadFontEx("assets/DejaVuSans.ttf", self->pileFontSize, symbolFontCodePoints, 5);

    flag_nodraw = 0;

    return self;
}

void PackDtor(struct Pack *self)
{
    if (self->ssFace) {
        SpritesheetFree(self->ssFace);
        self->ssFace = (void*)0;
    }
    if (self->ssBack) {
        SpritesheetFree(self->ssBack);
        self->ssBack = (void*)0;
    }
    if (self->unicodeFont.baseSize) {
        UnloadFont(self->unicodeFont);
        self->unicodeFont = (Font){0};
    }

    UnloadFont(self->pileFont);
    self->pileFont = (Font){0};
    UnloadFont(self->symbolFont);
    self->symbolFont = (Font){0};
}

