/* main.c */

#include <stdio.h>
#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>

#include "moon.h"
#include "baize.h"
#include "spritesheet.h"
#include "card.h"

struct Vector2 retroBackInfo[13] = {
    {.x = 5, .y = 4},
    {.x = 85, .y = 4},
    {.x = 185, .y = 4},
    {.x = 245, .y = 4},
    {.x = 325, .y = 4},
    {.x = 405, .y = 4},
    {.x = 485, .y = 4},
    {.x = 5, .y = 140},
    {.x = 85, .y = 140},
    {.x = 185, .y = 140},
    {.x = 245, .y = 140},
    {.x = 325, .y = 140},
    {.x = 405, .y = 140},
};

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

struct Spritesheet *ssFace, *ssBack;

int windowWidth = 1400, windowHeight = 1000;

//float cardWidth = 71.0f, cardHeight = 96.0f;
float cardWidth = 140.0f, cardHeight = 190.0f;

float pilePaddingX, pilePaddingY, leftMargin, topMargin;

Font fontAcme = {0};

// int main(int argc, char* argv[], char* envp[]);

// int main(int argc, char* argv[], char* envp[]) 
int main(void) 
{
    fprintf(stderr, "C version %ld\n", __STDC_VERSION__);

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

#if 0
    {
        lua_State *L = luaL_newstate();

        if ( luaL_loadfile(L, "csol.settings.lua") || lua_pcall(L, 0, 0, 0) ) {
            fprintf(stderr, "%s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        } else {
            windowWidth = MoonGetGlobalInt(L, "WindowWidth", 640);
            windowHeight = MoonGetGlobalInt(L, "WindowHeight", 480);
            int typ = lua_getglobal(L, "BaizeColor");
            if ( typ == LUA_TTABLE ) {
                float r, g, b, a;
                r = MoonGetFieldNumber(L, "red", 1);
                g = MoonGetFieldNumber(L, "green", 1);
                b = MoonGetFieldNumber(L, "blue", 1);
                a = MoonGetFieldNumber(L, "alpha", 1);
                baizeColor = (Color){.r=r*255, .g=g*255, .b=b*255, .a=a*255};
            } else {
                baizeColor = DARKGREEN;
            }
            lua_pop(L, 1);  // remove global (table)
        }

        lua_close(L);
    }
#endif
    InitWindow(windowWidth, windowHeight, "Oddstream Solitaire");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    // ssFace = SpritesheetNew("assets/cards71x96.png", 71, 96, 52, 13);
    // ssBack = SpritesheetNewInfo("assets/windows_16bit_cards.png", retroBackInfo, 13);
    ssFace = SpritesheetNewInfo("assets/playingCards.png", kenneyFaceInfo, 52);
    ssBack = SpritesheetNewInfo("assets/playingCardBacks.png", kenneyBackInfo, 15);
    
    struct Baize* baize = BaizeNew("Klondike");
    if ( BaizeValid(baize) ) {
        while (!WindowShouldClose())    // Detect window close button or ESC key
        {
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