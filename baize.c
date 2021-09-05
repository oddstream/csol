/* baize.c */

#include <stdlib.h>
#include <raylib.h>
#include "baize.h"
#include "card.h"

extern struct Card cardLibrary[52];

struct Baize* BaizeNew() {

    struct Baize* b = malloc(sizeof(struct Baize));
    b->piles = ArrayNew(16);
    b->stock = PileNew();
    ArrayPush(&b->piles, (void**)b->stock);
    // b->stock = (struct Pile*)ArrayGet(&b->piles, 0);

    struct Card* pc = &cardLibrary[0];
    PilePush(b->stock, pc);
    return b;
}

void BaizeUpdate(struct Baize* b) {

    static struct Card* currCard = NULL;
    static float dx, dy;

    struct Card* ace = PilePeek(b->stock);

    Vector2 touchPosition = GetTouchPosition(0);
    int gesture = GetGestureDetected();

    if ( gesture == GESTURE_TAP && CardIsAt(ace, touchPosition) ) {
        dx = touchPosition.x - ace->rect.x;
        dy = touchPosition.y - ace->rect.y;
        currCard = ace;
    }
    if ( gesture == GESTURE_DRAG && currCard ) {
        // card_position(&c, touchPosition.x - c.rect.width / 2, touchPosition.y - c.rect.height / 2);
        CardSetPosition(ace, (Vector2){touchPosition.x - dx, touchPosition.y - dy});
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

void BaizeDraw(struct Baize* b) {
    struct Card* ace = PilePeek(b->stock);

    ClearBackground(DARKGREEN);
    BeginDrawing();
    CardDraw(ace);
    DrawFPS(10, 10);
    EndDrawing();
}

void BaizeFree(struct Baize* b) {
    for ( int i = 0; i<ArrayLen(&b->piles); i++ ) {
        struct Pile* pp = (struct Pile*)ArrayGet(&b->piles, i);
        PileFree(pp);
    }
    free(b);
}
