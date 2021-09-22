/* main.c */

#include <stdio.h>
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

struct Spritesheet *ssFace, *ssBack;

int windowWidth = 2000, windowHeight = 1000;

#if RETROCARDS
float originalCardWidth = 71.0f, originalCardHeight = 96.0f;
#else
float originalCardWidth = 140.0f, originalCardHeight = 190.0f;
#endif

float cardScale = 1.0f;
float cardWidth, cardHeight;
float pilePaddingX, pilePaddingY, leftMargin, topMargin;
char variantName[64];

Color baizeColor;

// Font fontAcme = {0};

// int main(int argc, char* argv[], char* envp[]);

// int main(int argc, char* argv[], char* envp[]) 
int main(void) 
{
    fprintf(stderr, "C version %ld\n", __STDC_VERSION__);

    baizeColor = (Color){.r=0, .g=63, .b=0, .a=255};
    strncpy(variantName, "Klondike", sizeof(variantName)-1);
    LoadSettings();
    // fprintf(stderr, "cardScale %f\n", cardScale);

    cardWidth = originalCardWidth * cardScale;
    cardHeight = originalCardHeight * cardScale;
#if 0
    {
        int fontSizes[128-32];
        for ( int i=32; i<128; i++ ) {
            fontSizes[i] = i;
        }
        fontAcme = LoadFontEx("assets/Acme-Regular.ttf", 8, fontSizes, 128-32);
        // fontAcme = LoadFontEx("assets/DejaVuSans-Bold.ttf", 8, 0, 255);
        // fontAcme = LoadFontEx("assets/Roboto-Regular.ttf", 8, 0, 255);
    }
#endif

    // {
    //     int n = GetCurrentMonitor();
    //     int w = GetMonitorWidth(0);
    //     int h = GetMonitorHeight(0);
    //     fprintf(stderr, "%d:%d,%d\n", n, w, h);
    // }
    InitWindow(windowWidth, windowHeight, "Oddstream Solitaire");
#if 0
    {
        int n = GetCurrentMonitor();
        int w = GetMonitorWidth(n);
        int h = GetMonitorHeight(n);
        windowWidth =  w / 3 * 2;
        windowHeight =  h / 3 * 2;
        SetWindowSize(windowWidth, windowHeight);
        SetWindowPosition((w/2)-(windowWidth/2), (h/2)-(windowHeight/2));
    }
#endif
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

#if RETROCARDS
    ssFace = SpritesheetNew("assets/cards71x96.png", originalCardWidth, originalCardHeight, 13);
    ssBack = SpritesheetNewInfo("assets/windows_16bit_cards.png", retroBackInfo);
#else
    ssFace = SpritesheetNewInfo("assets/playingCards.png", kenneyFaceInfo);
    ssBack = SpritesheetNewInfo("assets/playingCardBacks.png", kenneyBackInfo);
#endif

    {
        fprintf(stdout, "Monitor %d,%d\n", GetMonitorWidth(0), GetMonitorHeight(0));
        fprintf(stdout, "Screen %d,%d\n", GetScreenWidth(), GetScreenHeight());
        fprintf(stdout, "Window %d,%d\n", windowWidth, windowHeight);
    }
    struct Baize* baize = BaizeNew();
    if ( BaizeValid(baize) ) {
        BaizeCreateCards(baize);
        BaizeCreatePiles(baize);
        BaizeResetState(baize);
        while ( !WindowShouldClose() ) {   // Detect window close button or ESC key
            BaizeUpdate(baize);
            BaizeDraw(baize);
        }
        BaizeFree(baize);
    }

    SpritesheetFree(ssFace);
    SpritesheetFree(ssBack);

    // UnloadFont(fontAcme);

    CloseWindow();        // Close window and OpenGL context

    return 0;
}
