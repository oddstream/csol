/* array.c */

#include <stdlib.h>

#include "array.h"

struct Array ArrayNew(size_t initialSize) {
    struct Array a = {.used = 0, .size = initialSize};
    a.array = calloc(initialSize, sizeof(void**));
    return a;
}

int ArrayLen(struct Array* a) {
    return a->used;
}

void** ArrayGet(struct Array* a, int pos) {
    return a->array[pos];
}

void ArrayPush(struct Array* a, void** element) {
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size 
    if ( a->used == a->size ) {
        a->size *= 2;
        a->array = reallocarray(a->array, a->size, sizeof(void**));
    }
    a->array[a->used++] = element;
}

void** ArrayPeek(struct Array* a) {
    if ( a->used == 0 ) {
        return NULL;
    }
    return a->array[a->used - 1];
}

void** ArrayPop(struct Array* a) {
    if ( a->used == 0 ) {
        return NULL;
    }
    return a->array[a->used-- - 1];
}

void ArrayFree(struct Array *a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}
