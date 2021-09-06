/* baize.c */

#include <stdlib.h>
#include <raylib.h>
#include "baize.h"
#include "card.h"

extern struct Card cardLibrary[52];

struct Baize* BaizeNew() {

    struct Baize* self = malloc(sizeof(struct Baize));
    self->piles = ArrayNew(16);

    self->stock = PileNew((Vector2){100, 100});
    ArrayPush(&self->piles, (void**)self->stock);
    PilePush(self->stock, &cardLibrary[0]);

    struct Pile* pf = PileNew((Vector2){200, 100});
    ArrayPush(&self->piles, (void**)pf);
    PilePush(pf, &cardLibrary[1]);

    pf = PileNew((Vector2){300, 100});
    ArrayPush(&self->piles, (void**)pf);
    PilePush(pf, &cardLibrary[2]);

    pf = PileNew((Vector2){400, 100});
    ArrayPush(&self->piles, (void**)pf);
    PilePush(pf, &cardLibrary[3]);

    pf = PileNew((Vector2){500, 100});
    ArrayPush(&self->piles, (void**)pf);
    PilePush(pf, &cardLibrary[4]);

    return self;
}

void BaizeUpdate(struct Baize* self) {

    (void)self;

    static struct Card* currCard = NULL;
    static float dx, dy;

    Vector2 touchPosition = GetTouchPosition(0);
    int gesture = GetGestureDetected();

    // TODO search piles from bottom to top, not this...
    struct Card* ace = NULL;
    for ( int i=0; i<52; i++ ) {
        struct Card* c = &cardLibrary[i];
        if ( CheckCollisionPointRec(touchPosition, c->rect) ) {
            ace = c;
            break;
        }
    }

    // if ( gesture == GESTURE_TAP && CardIsAt(ace, touchPosition) ) {
    if ( gesture == GESTURE_TAP && ace ) {
        dx = touchPosition.x - ace->rect.x;
        dy = touchPosition.y - ace->rect.y;
        currCard = ace;
    }
    if ( gesture == GESTURE_DRAG && currCard ) {
        // card_position(&c, touchPosition.x - c.rect.width / 2, touchPosition.y - c.rect.height / 2);
        CardSetPosition(currCard, (Vector2){touchPosition.x - dx, touchPosition.y - dy});
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

void BaizeDraw(struct Baize* self) {
    // struct Card* ace = PilePeek(b->stock);

    ClearBackground(DARKGREEN);
    BeginDrawing();
    int i = 0;
    struct Pile* p = (struct Pile*)ArrayFirst(&self->piles, &i);
    while ( p ) {
        PileDraw(p);
        int j = 0;
        struct Card* c = (struct Card*)ArrayFirst(&p->cards, &j);
        while ( c ) {
            CardDraw(c);
            c = (struct Card*)ArrayNext(&p->cards, &j);
        }
        p = (struct Pile*)ArrayNext(&self->piles, &i);
    }
    DrawFPS(10, 10);
    EndDrawing();
}

void BaizeFree(struct Baize* self) {
    for ( int i = 0; i<ArrayLen(&self->piles); i++ ) {
        struct Pile* pp = (struct Pile*)ArrayGet(&self->piles, i);
        PileFree(pp);
    }
    ArrayFree(&self->piles);
    free(self);
}
