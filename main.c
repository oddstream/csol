/* main.c */

#include <stdio.h>
#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>

#include "moon.h"
#include "baize.h"
#include "spritesheet.h"
#include "card.h"

struct Rectangle retroBackInfo[13] = {
    {.x = 5, .y = 4, .width = 71, .height = 96},
    {.x = 85, .y = 4, .width = 71, .height = 96},
    {.x = 185, .y = 4, .width = 71, .height = 96},
    {.x = 245, .y = 4, .width = 71, .height = 96},
    {.x = 325, .y = 4, .width = 71, .height = 96},
    {.x = 405, .y = 4, .width = 71, .height = 96},
    {.x = 485, .y = 4, .width = 71, .height = 96},
    {.x = 5, .y = 140, .width = 71, .height = 96},
    {.x = 85, .y = 140, .width = 71, .height = 96},
    {.x = 185, .y = 140, .width = 71, .height = 96},
    {.x = 245, .y = 140, .width = 71, .height = 96},
    {.x = 325, .y = 140, .width = 71, .height = 96},
    {.x = 405, .y = 140, .width = 71, .height = 96},
};

struct Spritesheet *ssFace, *ssBack;

float cardWidth = 71.0, cardHeight = 96.0;

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

    int windowWidth = 640 * 2, windowHeight = 480 * 2;

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

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    ssFace = SpritesheetNew("assets/cards71x96.png", 71, 96, 52, 13);
    ssBack = SpritesheetNewInfo("assets/windows_16bit_cards.png", retroBackInfo, 13);
    
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