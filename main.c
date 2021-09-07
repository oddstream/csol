/*******************************************************************************************
*
*   raylib [core] example - Basic 3d example
*
*   Welcome to raylib!
*
*   To compile example, just press F5.
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2013-2020 Ramon Santamaria (@raysan5)
*
********************************************************************************************/
#include <stdio.h>
#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>

#include "moon.h"
#include "baize.h"
#include "spritesheet.h"
#include "card.h"

struct SpriteInfo retroBackInfo[13] = {
    {.x = 5, .y = 4, .w = 71, .h = 96},
    {.x = 85, .y = 4, .w = 71, .h = 96},
    {.x = 185, .y = 4, .w = 71, .h = 96},
    {.x = 245, .y = 4, .w = 71, .h = 96},
    {.x = 325, .y = 4, .w = 71, .h = 96},
    {.x = 405, .y = 4, .w = 71, .h = 96},
    {.x = 485, .y = 4, .w = 71, .h = 96},
    {.x = 5, .y = 140, .w = 71, .h = 96},
    {.x = 85, .y = 140, .w = 71, .h = 96},
    {.x = 185, .y = 140, .w = 71, .h = 96},
    {.x = 245, .y = 140, .w = 71, .h = 96},
    {.x = 325, .y = 140, .w = 71, .h = 96},
    {.x = 405, .y = 140, .w = 71, .h = 96},
};

struct Spritesheet* ssFace;
struct Spritesheet* ssBack;

float cardWidth = 71.0, cardHeight = 96.0;

Color baizeColor;   // pedantically, can't initialize with a struct in C, because initializer element is not constant [-Werror=pedantic]

// int main(int argc, char* argv[], char* envp[]);

// int main(int argc, char* argv[], char* envp[]) 
int main(void) 
{
    int windowWidth, windowHeight;

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
        }

        lua_close(L);
    }

    InitWindow(windowWidth, windowHeight, "Oddstream Solitaire");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    ssFace = SpritesheetNew("assets/cards71x96.png", 71, 96, 52, 13);
    ssBack = SpritesheetNewInfo("assets/windows_16bit_cards.png", (struct SpriteInfo*)retroBackInfo, 13);
    
    struct Baize* baize = BaizeNew("Freecell");

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BaizeUpdate(baize);
        BaizeDraw(baize);
    }

    BaizeFree(baize);

    SpritesheetFree(ssFace);
    SpritesheetFree(ssBack);

    CloseWindow();        // Close window and OpenGL context

    return 0;
}