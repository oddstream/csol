/* pile.c */

#include <stdlib.h>
#include <raylib.h>
#include "pile.h"
#include "array.h"

struct Pile* PileNew() {
    struct Pile* p = malloc(sizeof(struct Pile));
    p->pos = (Vector2){10.0, 10.0};
    p->cards = ArrayNew(52);
    return p;
}

void PilePush(struct Pile* p, struct Card* pc) {
    ArrayPush(&p->cards, (void**)pc);
}

struct Card* PilePop(struct Pile* p) {
    return (struct Card*)ArrayPop(&p->cards);
}

struct Card* PilePeek(struct Pile* p) {
    return (struct Card*)ArrayPeek(&p->cards);
}

void PileUpdate(struct Pile* p) {
    (void)p;
}

void PileDraw(struct Pile* p) {
    (void)p;
}

void PileFree(struct Pile* p) {
    // Card objects exists in the cardLibrary array
    // for ( int i = 0; i<ArrayLen(&p->cards); i++ ) {
    //     struct Card* pc = ArrayGet(&p->cards, i);
    //     CardFree(pc);
    // }
    ArrayFree(&p->cards);
    free(p);
}
