/* main.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

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
struct Vector2 retroFaceInfo[52] = {
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

int windowWidth = 0, windowHeight = 0;

Font fontAcmePile = {0};
Font fontAcmeLabel = {0};
Font fontRobotoMedium24 = {0};
Font fontRobotoRegular14 = {0};
Font fontSymbol = {0};
int pileFontSize = 0;
int labelFontSize = 0;
float fontSpacing = 1.2f;

int flag_nolerp = 0;
int flag_noflip = 0;
int flag_noload = 0;
int flag_nosave = 0;

// int main(void) 
int main(int argc, char* argv[], char* envp[])
{
    (void)envp;

#if _DEBUG
    for ( int i=0; i<argc; i++ ) {
        fprintf(stdout, "%d. %s\n", i, argv[i]);
    }
#endif

    char variantName[32];
    memset(variantName, 0, sizeof(variantName));

    // https://azrael.digipen.edu/~mmead/www/Courses/CS180/getopt.html

    while (1) {
        static struct option long_options[] = {
            {"scale",   required_argument,  0,              's'},
            {"variant", required_argument,  0,              'v'},
            {"width",   required_argument,  0,              'w'},
            {"height",  required_argument,  0,              'h'},
            {"noload",  no_argument,        &flag_noload,   1},
            {"nosave",  no_argument,        &flag_nosave,   1},
            {"nolerp",  no_argument,        &flag_nolerp,   1},
            {"noflip",  no_argument,        &flag_noflip,   1},
            {NULL,      0,                  NULL,           0},
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "s:v:w:h:", long_options, &option_index);
        if (c == -1) {
            break;
        }
        /* K&R style switch formatting, see P59 if you don't believe me */
        switch (c) {

        case 0:
            if (long_options[option_index].flag != 0)
                break;
            fprintf(stdout, "INFO: %s: long option '%s' arg '%s'\n", __func__, long_options[option_index].name, optarg ? optarg : "null");
#if 0
                if (!optarg)
                    break;
                switch (long_options[option_index].val) {
                    case 's':
                        cardScale = atof(optarg);
                        break;
                    case 'v':
                        if (strlen(optarg) < 32) {
                            strcpy(variantName, optarg);
                        }
                        break;
                    case 'w':
                        if (atoi(optarg)) {
                            windowWidth = atoi(optarg);
                        }
                        break;
                    case 'h':
                        if (atoi(optarg)) {
                            windowHeight = atoi(optarg);
                        }
                        break;
                    default:
                        fprintf(stdout, "WARNING: %s: unhandled option %d\n", __func__, long_options[option_index].val);
                        break;
                }
#endif
            break;
        case 's':
            // fprintf(stdout, "INFO: %s: option s, value '%s'\n", __func__, optarg ? optarg : "null");
            cardScale = optarg ? atof(optarg) : 1.0f;
            break;
        case 'v':
            // fprintf(stdout, "INFO: %s: option v, value '%s'\n", __func__, optarg ? optarg : "null");
            if (optarg && strlen(optarg) < sizeof(variantName)) {
                strcpy(variantName, optarg);
            }
            break;
        case 'w':
            // fprintf(stdout, "INFO: %s: option w, value '%s'\n", __func__, optarg ? optarg : "null");
            if (optarg && atoi(optarg)) {
                windowWidth = atoi(optarg);
            }
            break;
        case 'h':
            // fprintf(stdout, "INFO: %s: option h, value '%s'\n", __func__, optarg ? optarg : "null");
            if (optarg && atoi(optarg)) {
                windowHeight = atoi(optarg);
            }
            break;
        case '?':
            // fprintf(stdout, "INFO: %s: already printed an error message\n", __func__);
            break;
        default:
            fprintf(stdout, "WARNING: %s: unhandled option %d\n", __func__, c);
            exit(1);
            break;
        }
     }

     if (optind < argc) {
         printf("Non option arguments: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
     }

#if 0
    fprintf(stderr, "C version %ld\n", __STDC_VERSION__);
    fprintf(stderr, "sizeof(int) == %lu\n", sizeof(int));
    fprintf(stderr, "sizeof(float) == %lu\n", sizeof(float));
    fprintf(stderr, "sizeof(void*) == %lu\n", sizeof(void*));
    fprintf(stderr, "sizeof(size_t) == %lu\n", sizeof(size_t));
    // fprintf(stderr, "sizeof(Array) == %lu\n", sizeof(struct Array));
    // fprintf(stderr, "sizeof(Card) == %lu\n", sizeof(struct Card));
    fprintf(stderr, "sizeof(Pile) == %lu\n", sizeof(struct Pile));
    fprintf(stderr, "sizeof(Baize) == %lu\n", sizeof(struct Baize));

    fprintf(stdout, "INFO: %s: nolerp=%d\n", __func__, flag_nolerp);
    fprintf(stdout, "INFO: %s: noload=%d\n", __func__, flag_noload);
    fprintf(stdout, "INFO: %s: nosave=%d\n", __func__, flag_nosave);
#endif

    baizeColor = (Color){.r=0, .g=63, .b=0, .a=255};
    baizeHighlightColor = (Color){255,255,255,31};
    uiBackgroundColor = (Color){.r=0x32, .g=0x32, .b=0x32, .a=0xee};
    uiTextColor = (Color){.r=0xf0, .g=0xf0, .b=0xf0, .a=0xff};

    // LoadSettings(&windowWidth, &windowHeight);
    // fprintf(stderr, "cardScale %f\n", cardScale);

    cardWidth = originalCardWidth * cardScale;
    cardHeight = originalCardHeight * cardScale;

    if (windowWidth == 0) {
        windowWidth = (int)cardWidth * 11;
    }
    if (windowHeight == 0) {
        windowHeight = (int)cardHeight * 5;
    }

    // {
    //     int n = GetCurrentMonitor();
    //     int w = GetMonitorWidth(0);
    //     int h = GetMonitorHeight(0);
    //     fprintf(stderr, "%d:%d,%d\n", n, w, h);
    // }
    InitWindow(windowWidth, windowHeight, "Oddstream Solitaire");

    {
        Image img = LoadImage("assets/appicon.png");
        SetWindowIcon(img);
        UnloadImage(img);
    }
    // NOTE: Textures/Fonts MUST be loaded after Window initialization (OpenGL context is required)
    pileFontSize = (int)(cardHeight / 2.0f);
    fontAcmePile = LoadFontEx("assets/Acme-Regular.ttf", pileFontSize, 0, 0);
    labelFontSize = (int)(cardHeight / 6.0f);
    fontAcmeLabel = LoadFontEx("assets/Acme-Regular.ttf", labelFontSize, 0, 0);
    fontRobotoMedium24 = LoadFontEx("assets/Roboto-Medium.ttf", 24, 0, 0);
    fontRobotoRegular14 = LoadFontEx("assets/Roboto-Regular.ttf", 14, 0, 0);

    // https://graphemica.com/search?q=home
    int codepoints[5] = {
        0x2663, /* black club */
        0x2666, /* black diamond */
        0x2665, /* black heart */
        0x2660, /* black spade */
        0x267b, /* recycle */ 
    };
    fontSymbol = LoadFontEx("assets/DejaVuSans-Bold.ttf", pileFontSize, codepoints, 5);

    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    // https://draeton.github.io/stitches/
    ssIcons = SpritesheetNew("assets/icons.png", 36, 36, 5);

#if RETROCARDS
    // ssFace = SpritesheetNew("assets/cards71x96.png", originalCardWidth, originalCardHeight, 13);
    ssFace = SpritesheetNewInfo("assets/lessblockycards71x96.png", originalCardWidth, originalCardHeight, retroFaceInfo);
    ssBack = SpritesheetNewInfo("assets/windows_16bit_cards.png", originalCardWidth, originalCardHeight, retroBackInfo);
#else
    ssFace = SpritesheetNewInfo("assets/playingCards.png", originalCardWidth, originalCardHeight, kenneyFaceInfo);
    ssBack = SpritesheetNewInfo("assets/playingCardBacks.png", originalCardWidth, originalCardHeight, kenneyBackInfo);
#endif

#if 0
    {
        fprintf(stdout, "Monitor %d,%d\n", GetMonitorWidth(0), GetMonitorHeight(0));
        fprintf(stdout, "Screen %d,%d\n", GetScreenWidth(), GetScreenHeight());
    }
#endif

    struct Array *loadedUndoStack = LoadUndoFromFile(variantName);
    if (loadedUndoStack) {
        if (variantName[0] == '\0') {
            fprintf(stderr, "ERROR: %s: variantName not set\n", __func__);
        }
        // that's fine
    } else {
        // strcpy(variantName, argc == 2 ? argv[1] : "Klondike");
        if (variantName[0] == '\0') {
            strcpy(variantName, "Klondike");
        }
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

    UnloadFont(fontAcmePile);
    UnloadFont(fontAcmeLabel);
    UnloadFont(fontSymbol);
    UnloadFont(fontRobotoMedium24);
    UnloadFont(fontRobotoRegular14);

    CloseWindow();

    return 0;
}
