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
#include "raylib.h"
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
struct Card cardLibrary[52];    // all references to Card objects are pointers into this array

// int main(int argc, char* argv[], char* envp[]);

// int main(int argc, char* argv[], char* envp[]) 
int main(void) 
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Oddstream Solitaire");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    ssFace = SpritesheetNew("assets/cards71x96.png", 71, 96, 52, 13);
    ssBack = SpritesheetNewInfo("assets/windows_16bit_cards.png", (struct SpriteInfo*)retroBackInfo, 13);
    
    {
        int i = 0;
        for ( enum CardSuit s = CLUB; s<=SPADE; s++ ) {
            for ( enum CardOrdinal o = ACE; o <= KING; o++ ) {
                cardLibrary[i++] = CardNew(ssFace, ssBack, s, o);
            }
        }
    }
    struct Baize* baize = BaizeNew();

    // c = CardNew(ssFace, ssBack, CLUB, ACE);
    // CardSetPosition(&c, (Vector2){100, 200});

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