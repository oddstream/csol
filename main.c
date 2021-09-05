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
struct Card c;

void BaizeInit(void);
void BaizeUpdate(void);
void BaizeDraw(void);
void BaizeFree(void);

void BaizeInit(void) {
    ssFace = SpritesheetNew("assets/cards71x96.png", 71, 96, 52, 13);
    ssBack = SpritesheetNewInfo("assets/windows_16bit_cards.png", (struct SpriteInfo*)retroBackInfo, 13);
}

void BaizeUpdate(void) {

    static struct Card* currCard = NULL;
    static float dx, dy;

    Vector2 touchPosition = GetTouchPosition(0);
    int gesture = GetGestureDetected();

    if ( gesture == GESTURE_TAP && CardIsAt(&c, touchPosition) ) {
        dx = touchPosition.x - c.rect.x;
        dy = touchPosition.y - c.rect.y;
        currCard = &c;
    }
    if ( gesture == GESTURE_DRAG && currCard ) {
        // card_position(&c, touchPosition.x - c.rect.width / 2, touchPosition.y - c.rect.height / 2);
        CardSetPosition(&c, (Vector2){touchPosition.x - dx, touchPosition.y - dy});
    }
    if ( gesture == GESTURE_NONE ) {
        currCard = NULL;
    }

    // if ( pc == NULL ) {
    //     if ( card_isAt(&c, touchPosition) ) {
    //         DrawText("touch on card", 0, 100, 16, WHITE);
    //         pc = &c;
    //     }
    // }

    // int dx = c.rect.x - touchPosition.x;
    // int dy = c.rect.y - touchPosition.y;
    // {
    //     char buf[64];
    //     sprintf(buf, "dragging %d,%d", dx, dy);
    //     DrawText(buf, 0, 130, 16, WHITE);
    // }
    // card_position(&c, touchPosition.x - dx, touchPosition.y - dy);
}

void BaizeDraw(void) {
    ClearBackground(DARKGREEN);
    BeginDrawing();
    CardDraw(&c);
    DrawFPS(10, 10);
    EndDrawing();
}

void BaizeFree(void) {
    SpritesheetFree(ssFace);
    SpritesheetFree(ssBack);
}


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

    BaizeInit();

    c = CardNew(ssFace, ssBack, CLUB, ACE);
    CardSetPosition(&c, (Vector2){100, 200});

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BaizeUpdate();
        BaizeDraw();
    }

    BaizeFree();

    CloseWindow();        // Close window and OpenGL context

    return 0;
}