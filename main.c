/* main.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <raylib.h>

#include "baize.h"
#include "command.h"
#include "spritesheet.h"
#include "trace.h"
#include "undo.h"

float pilePaddingX, pilePaddingY, leftMargin, topMargin;

Color baizeColor, baizeHighlightColor, uiBackgroundColor, uiTextColor;

float fontSpacing = 1.2f;

int flag_nolerp = 0;
int flag_noflip = 0;
int flag_noload = 0;
int flag_nosave = 0;
int flag_noshuf = 0;
int flag_nodraw = 0;

// int main(void) 
int main(int argc, char* argv[], char* envp[])
{
    (void)envp;

#ifdef _DEBUG
    for ( int i=0; i<argc; i++ ) {
        CSOL_INFO("%d. %s\n", i, argv[i]);
    }
#endif

    char variantName[32];       memset(variantName, 0, sizeof(variantName));
    char packName[32];          memset(packName, 0, sizeof(packName));
    int windowWidth = 0, windowHeight = 0;

    // https://azrael.digipen.edu/~mmead/www/Courses/CS180/getopt.html

    while (1) {
        static struct option long_options[] = {
            {"pack",    required_argument,  0,                  'p'},
            {"scale",   required_argument,  0,                  's'},   // deprecated
            {"variant", required_argument,  0,                  'v'},
            {"width",   required_argument,  0,                  'w'},
            {"height",  required_argument,  0,                  'h'},
            {"noload",  no_argument,        &flag_noload,       1},
            {"nosave",  no_argument,        &flag_nosave,       1},
            {"nolerp",  no_argument,        &flag_nolerp,       1},
            {"noflip",  no_argument,        &flag_noflip,       1},
            {"noshuf",  no_argument,        &flag_noshuf,       1},
            // flag_nodraw turns off drawing while a pack change is happening; no sense setting it here
            {NULL,      0,                  NULL,               0},
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
            CSOL_INFO("long option '%s' arg '%s'\n", long_options[option_index].name, optarg ? optarg : "null");
#if 0
                if (!optarg)
                    break;
                switch (long_options[option_index].val) {
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
        case 'p':
            // fprintf(stdout, "INFO: %s: option c, value '%s'\n", __func__, optarg ? optarg : "null");
            if (optarg && strlen(optarg) < sizeof(packName)) {
                strcpy(packName, optarg);
            }
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
            CSOL_WARNING("unhandled option %d", c);
            exit(1);
            break;
        }
     }

#ifdef _DEBUG
     if (optind < argc) {
         printf("Non option arguments: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
     }
#endif

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

    baizeColor = (Color){.r=0, .g=70, .b=0, .a=255};   // darker than darkgreen
    baizeHighlightColor = (Color){255,255,255,25};
    uiBackgroundColor = (Color){.r=0x32, .g=0x32, .b=0x32, .a=0xee};
    uiTextColor = (Color){.r=0xf0, .g=0xf0, .b=0xf0, .a=0xff};

    // pearl from the mudbank: can't use GetMonitorWidth() &c until InitWindow is called!

    // LoadSettings(&windowWidth, &windowHeight);

    // {
    //     int n = GetCurrentMonitor();
    //     int w = GetMonitorWidth(0);
    //     int h = GetMonitorHeight(0);
    //     fprintf(stderr, "%d:%d,%d\n", n, w, h);
    // }

    InitWindow(640*2, 480*2, "Oddstream Solitaire");

#ifdef PLATFORM_DESKTOP
    if (windowWidth == 0 || windowHeight == 0) {
        windowWidth = GetMonitorWidth(GetCurrentMonitor()) * 2 / 3;
        windowHeight = GetMonitorHeight(GetCurrentMonitor()) * 2 / 3;
        SetWindowSize(windowWidth, windowHeight);
    }
    {
        Image img = LoadImage("assets/appicon.png");
        SetWindowIcon(img);
        UnloadImage(img);
    }
    SetWindowState(FLAG_WINDOW_RESIZABLE);
#endif

    SetTargetFPS(60);

    // NOTE: Textures/Fonts MUST be loaded after Window initialization (OpenGL context is required)

#if 0
    {
        fprintf(stdout, "Monitor %d,%d\n", GetMonitorWidth(0), GetMonitorHeight(0));
        fprintf(stdout, "Screen %d,%d\n", GetScreenWidth(), GetScreenHeight());
    }
#endif

    struct Array *loadedUndoStack = LoadUndoFromFile(variantName);
    if (loadedUndoStack) {
        if (variantName[0] == '\0') {
            CSOL_ERROR("%s", "variantName not set");
        }
        // that's fine
    } else {
        // strcpy(variantName, argc == 2 ? argv[1] : "Klondike");
        if (variantName[0] == '\0') {
            strcpy(variantName, "Klondike");
        }
        CSOL_INFO("starting a fresh '%s'", variantName);
    }

    struct Baize* baize = BaizeNew(variantName, packName);
    if ( BaizeValid(baize) ) {
        StartCommandQueue();

        BaizeOpenLua(baize);
        BaizeCreatePiles(baize);
        BaizeResetState(baize, loadedUndoStack);  // Baize takes ownership of loadedUndoStack
        if (loadedUndoStack) {
            BaizeUndo0(baize);
            if (ArrayLen(baize->undoStack) == 1) {
                baize->driface->StartGame(baize);
            }
        } else {
            baize->driface->StartGame(baize);
        }
        BaizeUndoPush(baize);

        while ( !WindowShouldClose() ) {   // Detect window close button or ESC key
            if (flag_nodraw)
                continue;   // eg while changing packs
            BaizeLayout(baize);
            BaizeUpdate(baize);
            BaizeDraw(baize);
            ServiceCommandQueue(baize);
        }
        StopCommandQueue();

        BaizeSaveUndoToFile(baize);
        BaizeCloseLua(baize);
        BaizeFree(baize);
    }

    CloseWindow();

    return 0;
}
