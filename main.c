/* main.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>

#include "baize.h"
#include "spritesheet.h"
#include "settings.h"

#define RETROCARDS 0

#if RETROCARDS
struct Vector2 retroBackInfo[13] = {
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
#else
struct Vector2 kenneyFaceInfo[52] = {
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

struct Vector2 kenneyBackInfo[15] = {
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

struct Spritesheet *ssFace, *ssBack, *ssIcons;

#if RETROCARDS
float originalCardWidth = 71.0f, originalCardHeight = 96.0f;
#else
float originalCardWidth = 140.0f, originalCardHeight = 190.0f;
#endif

float cardScale = 1.0f;
float cardWidth, cardHeight;
float pilePaddingX, pilePaddingY, leftMargin, topMargin;
char variantName[64] = "Klondike";

Color baizeColor, baizeHighlightColor, uiBackgroundColor, uiTextColor;

Font fontAcme = {0};
Font fontRobotoMedium24 = {0};
Font fontRobotoRegular14 = {0};

struct Array* BaizeCommandQueue = {0};

// int main(int argc, char* argv[], char* envp[]);

// int main(int argc, char* argv[], char* envp[]) 
int main(void) 
{
    int windowWidth = 640, windowHeight = 480;

    fprintf(stderr, "C version %ld\n", __STDC_VERSION__);
    fprintf(stderr, "sizeof(void*) == %lu\n", sizeof(void*));
    fprintf(stderr, "sizeof(Baize) == %lu\n", sizeof(struct Baize));
    fprintf(stderr, "sizeof(Card) == %lu\n", sizeof(struct Card));

    baizeColor = (Color){.r=0, .g=63, .b=0, .a=255};
    baizeHighlightColor = (Color){255,255,255,31};
    uiBackgroundColor = (Color){.r=0x32, .g=0x32, .b=0x32, .a=0xff};
    uiTextColor = (Color){.r=0xf0, .g=0xf0, .b=0xf0, .a=0xff};

    LoadSettings(&windowWidth, &windowHeight);
    // fprintf(stderr, "cardScale %f\n", cardScale);

    cardWidth = originalCardWidth * cardScale;
    cardHeight = originalCardHeight * cardScale;

    {
        // int fontSizes[128-32];
        // for ( int i=32; i<128; i++ ) {
        //     fontSizes[i] = i;
        // }
        // fontAcme = LoadFontEx("assets/KAISG.ttf", 96, 0, 0);
        // fontAcme = LoadFontEx("assets/Acme-Regular.ttf", 8, fontSizes, 128-32);
        // fontAcme = LoadFontEx("assets/DejaVuSans-Bold.ttf", 8, 0, 255);
        // fontAcme = LoadFontEx("assets/Roboto-Regular.ttf", 8, 0, 255);
    }


    // {
    //     int n = GetCurrentMonitor();
    //     int w = GetMonitorWidth(0);
    //     int h = GetMonitorHeight(0);
    //     fprintf(stderr, "%d:%d,%d\n", n, w, h);
    // }
    InitWindow(windowWidth, windowHeight, "Oddstream Solitaire");

    // NOTE: Textures/Fonts MUST be loaded after Window initialization (OpenGL context is required)
    fontAcme = LoadFontEx("assets/Acme-Regular.ttf", (int)cardWidth / 2, 0, 0);
    fontRobotoMedium24 = LoadFontEx("assets/Roboto-Medium.ttf", 24, 0, 0);
    fontRobotoRegular14 = LoadFontEx("assets/Roboto-Regular.ttf", 14, 0, 0);

    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    // https://draeton.github.io/stitches/
    ssIcons = SpritesheetNew("assets/icons.png", 36, 36, 5);

#if RETROCARDS
    ssFace = SpritesheetNew("assets/cards71x96.png", originalCardWidth, originalCardHeight, 13);
    ssBack = SpritesheetNewInfo("assets/windows_16bit_cards.png", originalCardWidth, originalCardHeight, retroBackInfo);
#else
    ssFace = SpritesheetNewInfo("assets/playingCards.png", originalCardWidth, originalCardHeight, kenneyFaceInfo);
    ssBack = SpritesheetNewInfo("assets/playingCardBacks.png", originalCardWidth, originalCardHeight, kenneyBackInfo);
#endif

    {
        fprintf(stdout, "Monitor %d,%d\n", GetMonitorWidth(0), GetMonitorHeight(0));
        fprintf(stdout, "Screen %d,%d\n", GetScreenWidth(), GetScreenHeight());
    }
    struct Baize* baize = BaizeNew();
    if ( BaizeValid(baize) ) {
        BaizeOpenLua(baize);
        BaizeCreateCards(baize);
        BaizeCreatePiles(baize);
        BaizeResetState(baize);
        BaizeStartGame(baize);
        BaizeCommandQueue = ArrayNew(8);
        while ( !WindowShouldClose() ) {   // Detect window close button or ESC key
            BaizeLayout(baize, GetScreenWidth(), GetScreenHeight());
            BaizeUpdate(baize);
            BaizeDraw(baize);
            if ( ArrayLen(BaizeCommandQueue) > 0 ) {
                // ISO C forbids conversion of object pointer to function pointer type [-Werror=pedantic]
                struct BaizeCommand *bc = ArrayGet(BaizeCommandQueue, 0);
                if ( bc ) {
                    if ( bc->bcf ) {
                        bc->bcf(baize, bc->param);
                    }
                    ArrayDeleteFirst(BaizeCommandQueue, free);
                } else {
                    ArrayDeleteFirst(BaizeCommandQueue, NULL);
                }
            }
        }
        ArrayFree(BaizeCommandQueue);
        BaizeCloseLua(baize);
        BaizeFree(baize);
    }

    SpritesheetFree(ssFace);
    SpritesheetFree(ssBack);
    SpritesheetFree(ssIcons);

    UnloadFont(fontAcme);
    UnloadFont(fontRobotoMedium24);
    UnloadFont(fontRobotoRegular14);

    CloseWindow();        // Close window and OpenGL context

    return 0;
}
