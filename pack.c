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
    {.x= 1*150-150, .y=1*200-200},     // Ace
    {.x= 2*150-150, .y=1*200-200},     // 2
    {.x= 3*150-150, .y=1*200-200},     // 3
    {.x= 4*150-150, .y=1*200-200},     // 4
    {.x= 5*150-150, .y=1*200-200},     // 5
    {.x= 6*150-150, .y=1*200-200},     // 6
    {.x= 7*150-150, .y=1*200-200},     // 7
    {.x= 8*150-150, .y=1*200-200},     // 8
    {.x= 9*150-150, .y=1*200-200},     // 9
    {.x=10*150-150, .y=1*200-200},     // 10
    {.x=11*150-150, .y=1*200-200},     // J
    {.x=12*150-150, .y=1*200-200},     // Q
    {.x=13*150-150, .y=1*200-200},     // K
};

static struct Vector2 kenney96BackMap[15] = {
    {.x=1*150-150, .y=1*200-200},
    {.x=2*150-150, .y=1*200-200},
    {.x=3*150-150, .y=1*200-200},
    {.x=4*150-150, .y=1*200-200},
    {.x=5*150-150, .y=1*200-200},

    {.x=1*150-150, .y=2*200-200},
    {.x=2*150-150, .y=2*200-200},
    {.x=3*150-150, .y=2*200-200},
    {.x=4*150-150, .y=2*200-200},
    {.x=5*150-150, .y=2*200-200},

    {.x=1*150-150, .y=3*200-200},
    {.x=2*150-150, .y=3*200-200},
    {.x=3*150-150, .y=3*200-200},
    {.x=4*150-150, .y=3*200-200},
    {.x=5*150-150, .y=3*200-200},
};

static struct Vector2 kenney64FaceMap[52] = {
    // Club
    {.x= 1*100-100, .y=2*133-133},     // Ace
    {.x= 2*100-100, .y=2*133-133},     // 2
    {.x= 3*100-100, .y=2*133-133},     // 3
    {.x= 4*100-100, .y=2*133-133},     // 4
    {.x= 5*100-100, .y=2*133-133},     // 5
    {.x= 6*100-100, .y=2*133-133},     // 6
    {.x= 7*100-100, .y=2*133-133},     // 7
    {.x= 8*100-100, .y=2*133-133},     // 8
    {.x= 9*100-100, .y=2*133-133},     // 9
    {.x=10*100-100, .y=2*133-133},     // 10
    {.x=11*100-100, .y=2*133-133},     // J
    {.x=12*100-100, .y=2*133-133},     // Q
    {.x=13*100-100, .y=2*133-133},     // K
    // Diamond
    {.x= 1*100-100, .y=3*133-133},     // Ace
    {.x= 2*100-100, .y=3*133-133},     // 2
    {.x= 3*100-100, .y=3*133-133},     // 3
    {.x= 4*100-100, .y=3*133-133},     // 4
    {.x= 5*100-100, .y=3*133-133},     // 5
    {.x= 6*100-100, .y=3*133-133},     // 6
    {.x= 7*100-100, .y=3*133-133},     // 7
    {.x= 8*100-100, .y=3*133-133},     // 8
    {.x= 9*100-100, .y=3*133-133},     // 9
    {.x=10*100-100, .y=3*133-133},     // 10
    {.x=11*100-100, .y=3*133-133},     // J
    {.x=12*100-100, .y=3*133-133},     // Q
    {.x=13*100-100, .y=3*133-133},     // K
    // Heart
    {.x= 1*100-100, .y=4*133-133},     // Ace
    {.x= 2*100-100, .y=4*133-133},     // 2
    {.x= 3*100-100, .y=4*133-133},     // 3
    {.x= 4*100-100, .y=4*133-133},     // 4
    {.x= 5*100-100, .y=4*133-133},     // 5
    {.x= 6*100-100, .y=4*133-133},     // 6
    {.x= 7*100-100, .y=4*133-133},     // 7
    {.x= 8*100-100, .y=4*133-133},     // 8
    {.x= 9*100-100, .y=4*133-133},     // 9
    {.x=10*100-100, .y=4*133-133},     // 10
    {.x=11*100-100, .y=4*133-133},     // J
    {.x=12*100-100, .y=4*133-133},     // Q
    {.x=13*100-100, .y=4*133-133},     // K
    // Spade
    {.x= 1*100-100, .y=1*133-133},     // Ace
    {.x= 2*100-100, .y=1*133-133},     // 2
    {.x= 3*100-100, .y=1*133-133},     // 3
    {.x= 4*100-100, .y=1*133-133},     // 4
    {.x= 5*100-100, .y=1*133-133},     // 5
    {.x= 6*100-100, .y=1*133-133},     // 6
    {.x= 7*100-100, .y=1*133-133},     // 7
    {.x= 8*100-100, .y=1*133-133},     // 8
    {.x= 9*100-100, .y=1*133-133},     // 9
    {.x=10*100-100, .y=1*133-133},     // 10
    {.x=11*100-100, .y=1*133-133},     // J
    {.x=12*100-100, .y=1*133-133},     // Q
    {.x=13*100-100, .y=1*133-133},     // K
};

static struct Vector2 kenney64BackMap[15] = {
    {.x=1*100-100, .y=1*133-133},
    {.x=2*100-100, .y=1*133-133},
    {.x=3*100-100, .y=1*133-133},
    {.x=4*100-100, .y=1*133-133},
    {.x=5*100-100, .y=1*133-133},

    {.x=1*100-100, .y=2*133-133},
    {.x=2*100-100, .y=2*133-133},
    {.x=3*100-100, .y=2*133-133},
    {.x=4*100-100, .y=2*133-133},
    {.x=5*100-100, .y=2*133-133},

    {.x=1*100-100, .y=3*133-133},
    {.x=2*100-100, .y=3*133-133},
    {.x=3*100-100, .y=3*133-133},
    {.x=4*100-100, .y=3*133-133},
    {.x=5*100-100, .y=3*133-133},
};

static struct Vector2 kenney46FaceMap[52] = {
    // Club
    {.x= 1*72-72, .y=2*95-95},     // Ace
    {.x= 2*72-72, .y=2*95-95},     // 2
    {.x= 3*72-72, .y=2*95-95},     // 3
    {.x= 4*72-72, .y=2*95-95},     // 4
    {.x= 5*72-72, .y=2*95-95},     // 5
    {.x= 6*72-72, .y=2*95-95},     // 6
    {.x= 7*72-72, .y=2*95-95},     // 7
    {.x= 8*72-72, .y=2*95-95},     // 8
    {.x= 9*72-72, .y=2*95-95},     // 9
    {.x=10*72-72, .y=2*95-95},     // 10
    {.x=11*72-72, .y=2*95-95},     // J
    {.x=12*72-72, .y=2*95-95},     // Q
    {.x=13*72-72, .y=2*95-95},     // K
    // Diamond
    {.x= 1*72-72, .y=3*95-95},     // Ace
    {.x= 2*72-72, .y=3*95-95},     // 2
    {.x= 3*72-72, .y=3*95-95},     // 3
    {.x= 4*72-72, .y=3*95-95},     // 4
    {.x= 5*72-72, .y=3*95-95},     // 5
    {.x= 6*72-72, .y=3*95-95},     // 6
    {.x= 7*72-72, .y=3*95-95},     // 7
    {.x= 8*72-72, .y=3*95-95},     // 8
    {.x= 9*72-72, .y=3*95-95},     // 9
    {.x=10*72-72, .y=3*95-95},     // 10
    {.x=11*72-72, .y=3*95-95},     // J
    {.x=12*72-72, .y=3*95-95},     // Q
    {.x=13*72-72, .y=3*95-95},     // K
    // Heart
    {.x= 1*72-72, .y=4*95-95},     // Ace
    {.x= 2*72-72, .y=4*95-95},     // 2
    {.x= 3*72-72, .y=4*95-95},     // 3
    {.x= 4*72-72, .y=4*95-95},     // 4
    {.x= 5*72-72, .y=4*95-95},     // 5
    {.x= 6*72-72, .y=4*95-95},     // 6
    {.x= 7*72-72, .y=4*95-95},     // 7
    {.x= 8*72-72, .y=4*95-95},     // 8
    {.x= 9*72-72, .y=4*95-95},     // 9
    {.x=10*72-72, .y=4*95-95},     // 10
    {.x=11*72-72, .y=4*95-95},     // J
    {.x=12*72-72, .y=4*95-95},     // Q
    {.x=13*72-72, .y=4*95-95},     // K
    // Spade
    {.x= 1*72-72, .y=1*95-95},     // Ace
    {.x= 2*72-72, .y=1*95-95},     // 2
    {.x= 3*72-72, .y=1*95-95},     // 3
    {.x= 4*72-72, .y=1*95-95},     // 4
    {.x= 5*72-72, .y=1*95-95},     // 5
    {.x= 6*72-72, .y=1*95-95},     // 6
    {.x= 7*72-72, .y=1*95-95},     // 7
    {.x= 8*72-72, .y=1*95-95},     // 8
    {.x= 9*72-72, .y=1*95-95},     // 9
    {.x=10*72-72, .y=1*95-95},     // 10
    {.x=11*72-72, .y=1*95-95},     // J
    {.x=12*72-72, .y=1*95-95},     // Q
    {.x=13*72-72, .y=1*95-95},     // K
};

static struct Vector2 kenney46BackMap[15] = {
    {.x=1*72-72, .y=1*95-95},
    {.x=2*72-72, .y=1*95-95},
    {.x=3*72-72, .y=1*95-95},
    {.x=4*72-72, .y=1*95-95},
    {.x=5*72-72, .y=1*95-95},

    {.x=1*72-72, .y=2*95-95},
    {.x=2*72-72, .y=2*95-95},
    {.x=3*72-72, .y=2*95-95},
    {.x=4*72-72, .y=2*95-95},
    {.x=5*72-72, .y=2*95-95},

    {.x=1*72-72, .y=3*95-95},
    {.x=2*72-72, .y=3*95-95},
    {.x=3*72-72, .y=3*95-95},
    {.x=4*72-72, .y=3*95-95},
    {.x=5*72-72, .y=3*95-95},
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
        .width = 93.0f,
        .height = 126.0f,
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
        .width = 66.0f,
        .height = 90.0f,
        .roundness = 0.05f,
        .ssFaceMap = kenney46FaceMap,
        .ssBackMap = kenney46BackMap,
        .backMapEntries = 15,
        .backFrame = 14,
        .ssFaceFname = "assets/playingCards46.png",
        .ssBackFname = "assets/playingCardBacks46.png",
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
    if (self->font.baseSize) {
        UnloadFont(self->font);
        self->font = (Font){0};
    }

    UnloadFont(self->pileFont);
    self->pileFont = (Font){0};
    UnloadFont(self->symbolFont);
    self->symbolFont = (Font){0};
}

