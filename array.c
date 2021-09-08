/* array.c */

#include <stdlib.h>

#include "array.h"

struct Array* ArrayNew(size_t initialSize) {
    struct Array* self = malloc(sizeof(struct Array));
    self->used = 0;
    self->size = initialSize;
    self->array = calloc(initialSize, sizeof(void**));
    return self;
}

int ArrayLen(struct Array* self) {
    return self->used;
}

void** ArrayGet(struct Array* self, int pos) {
    return self->array[pos];
}

void** ArrayFirst(struct Array* self, int* savedPos) {
    if ( savedPos ) {
        *savedPos = 0;
    }
    return self->array[0];
}

void** ArrayNext(struct Array* self, int* savedPos) {
    *savedPos = *savedPos + 1;
    return self->array[*savedPos];
}

void** ArrayPrev(struct Array* self, int* savedPos) {
    if ( *savedPos == 0 ) {
        return NULL;
    }
    *savedPos = *savedPos - 1;
    return self->array[*savedPos];
}

void** ArrayLast(struct Array* self, int* savedPos) {
    if ( self->used == 0 ) {
        return NULL;
    }
    *savedPos = self->used - 1;
    return self->array[*savedPos];
}

void ArrayPush(struct Array* self, void** element) {
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size 
    if ( self->used == self->size ) {
        self->size *= 2;
        self->array = reallocarray(self->array, self->size, sizeof(void**));
    }
    self->array[self->used++] = element;
}

void** ArrayPeek(struct Array* self) {
    if ( self->used == 0 ) {
        return NULL;
    }
    return self->array[self->used - 1];
}

void** ArrayPop(struct Array* self) {
    if ( self->used == 0 ) {
        return NULL;
    }
    return self->array[self->used-- - 1];
}

void ArrayForeach(struct Array *self, ArrayIterFunc f) {
    for ( size_t i = 0; i<self->used; i++ ) {
        f(self->array[i]);
    }
}

void ArrayFree(struct Array *self) {
    free(self->array);
    free(self);
}
