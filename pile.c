/* pile.c */

#include <stdlib.h>
#include <raylib.h>
#include "pile.h"
#include "array.h"

struct Pile* PileNew(Vector2 pos) {
    struct Pile* p = malloc(sizeof(struct Pile));
    p->pos = pos;
    p->cards = ArrayNew(52);
    return p;
}

void PilePush(struct Pile* p, struct Card* c) {
    CardSetOwner(c, p);
    CardSetPosition(c, p->pos);
    ArrayPush(&p->cards, (void**)c);
}

struct Card* PilePop(struct Pile* p) {
    struct Card* c = (struct Card*)ArrayPop(&p->cards);
    CardSetOwner(c, NULL);
    return c;
}

struct Card* PilePeek(struct Pile* p) {
    return (struct Card*)ArrayPeek(&p->cards);
}

Vector2 PileGetPosition(struct Pile* p) {
    return p->pos;
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
