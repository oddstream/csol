/* main.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>

#include "baize.h"
#include "command.h"
#include "spritesheet.h"
#include "settings.h"
#include "undo.h"

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

Color baizeColor, baizeHighlightColor, uiBackgroundColor, uiTextColor;

Font fontAcme = {0};
Font fontRobotoMedium24 = {0};
Font fontRobotoRegular14 = {0};
Texture2D recycleTexture = {0};

int main(int argc, char* argv[], char* envp[])
// int main(void) 
{
    (void)envp;

#if _DEBUG
    fprintf(stderr, "C version %ld\n", __STDC_VERSION__);
    fprintf(stderr, "sizeof(int) == %lu\n", sizeof(int));
    fprintf(stderr, "sizeof(float) == %lu\n", sizeof(float));
    fprintf(stderr, "sizeof(void*) == %lu\n", sizeof(void*));
    fprintf(stderr, "sizeof(size_t) == %lu\n", sizeof(size_t));
    fprintf(stderr, "sizeof(Array) == %lu\n", sizeof(struct Array));
    fprintf(stderr, "sizeof(Card) == %lu\n", sizeof(struct Card));
    fprintf(stderr, "sizeof(Pile) == %lu\n", sizeof(struct Pile));
    fprintf(stderr, "sizeof(Baize) == %lu\n", sizeof(struct Baize));

    for ( int i=0; i<argc; i++ ) {
        fprintf(stdout, "%d. %s\n", i, argv[i]);
    }
#endif

    int windowWidth = 640, windowHeight = 480;

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

    {
        Image img = LoadImage("assets/outline_recycling_white_48dp.png");
        SetWindowIcon(img);
        UnloadImage(img);
    }
    // NOTE: Textures/Fonts MUST be loaded after Window initialization (OpenGL context is required)
    fontAcme = LoadFontEx("assets/Acme-Regular.ttf", (int)cardWidth / 2, 0, 0);
    fontRobotoMedium24 = LoadFontEx("assets/Roboto-Medium.ttf", 24, 0, 0);
    fontRobotoRegular14 = LoadFontEx("assets/Roboto-Regular.ttf", 14, 0, 0);

    recycleTexture = LoadTexture("assets/outline_recycling_white_48dp.png");

    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    // https://draeton.github.io/stitches/
    ssIcons = SpritesheetNew("assets/icons.png", 36, 36, 5);

#if RETROCARDS
    ssFace = SpritesheetNew("assets/cards71x96.png", originalCardWidth, originalCardHeight, 13);
    ssBack = SpritesheetNewInfo("assets/windows_16bit_cards.png", originalCardWidth, originalCardHeight, retroBackInfo);
#else
    ssFace = SpritesheetNewInfo("assets/playingCards.png", originalCardWidth, originalCardHeight, kenneyFaceInfo);
    ssBack = SpritesheetNewInfo("assets/playingCardBacks.png", originalCardWidth, originalCardHeight, kenneyBackInfo);
#endif

#if _DEBUG
    {
        fprintf(stdout, "Monitor %d,%d\n", GetMonitorWidth(0), GetMonitorHeight(0));
        fprintf(stdout, "Screen %d,%d\n", GetScreenWidth(), GetScreenHeight());
    }
#endif

    char variantName[32];
    struct Array *loadedUndoStack = LoadUndoFromFile(variantName);
    if (loadedUndoStack) {
        fprintf(stdout, "INFO: %s: state loaded from file for '%s'\n", __func__, variantName);
        if (variantName[0] == '\0') {
            fprintf(stderr, "ERROR: %s: variantName not set\n", __func__);
        }
        // that's fine
    } else {
        strcpy(variantName, argc == 2 ? argv[1] : "Klondike");
        fprintf(stdout, "INFO: %s: starting a fresh '%s'\n", __func__, variantName);
    }

    struct Baize* baize = BaizeNew(variantName);
    if ( BaizeValid(baize) ) {
        BaizeOpenLua(baize);
        BaizeCreatePiles(baize);
        BaizeResetState(baize, loadedUndoStack);  // Baize takes ownership of loadedUndoStack
        if (loadedUndoStack) {
            BaizeUndo0(baize);
            if (ArrayLen(baize->undoStack) == 1) {
                BaizeStartGame(baize);
            }
        } else {
            BaizeStartGame(baize);
        }
        BaizeGetLuaGlobals(baize);
        BaizeUndoPush(baize);

        StartCommandQueue();
        while ( !WindowShouldClose() ) {   // Detect window close button or ESC key
            BaizeLayout(baize, GetScreenWidth(), GetScreenHeight());
            BaizeUpdate(baize);
            BaizeDraw(baize);
            ServiceCommandQueue(baize);
        }
        StopCommandQueue();

        BaizeSaveUndoToFile(baize);
        BaizeCloseLua(baize);
        BaizeFree(baize);
    }

    SpritesheetFree(ssFace);
    SpritesheetFree(ssBack);
    SpritesheetFree(ssIcons);

    UnloadTexture(recycleTexture);

    UnloadFont(fontAcme);
    UnloadFont(fontRobotoMedium24);
    UnloadFont(fontRobotoRegular14);

    CloseWindow();

    return 0;
}
