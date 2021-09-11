/* array.c */

#include <stdlib.h>
#include <string.h>

#include "array.h"

struct Array* ArrayNew(size_t initialSize) {
    struct Array* self = malloc(sizeof(struct Array));
    self->used = 0;
    self->size = initialSize;
    self->array = calloc(initialSize, sizeof(void**));
    return self;
}

size_t ArrayLen(struct Array *const self) {
    return self->used;
}

size_t ArrayCap(struct Array *const self) {
    return self->size;
}

void ArraySwap(struct Array *const self, int i, int j) {
    void** tmp = self->array[i];
    self->array[i] = self->array[j];
    self->array[j] = tmp;
}

void** ArrayGet(struct Array *const self, int pos) {
    // deprecated
    return self->array[pos];
}

void** ArrayFirst(struct Array *const self, size_t *index) {
    if ( self->used == 0 ) {
        return NULL;
    }
    *index = 0;
    return self->array[0];
}

void** ArrayNext(struct Array *const self, size_t *index) {
    *index += 1;
    if ( *index >= self->used ) {
        return NULL;
    }
    return self->array[*index];
}

void** ArrayPrev(struct Array *const self, size_t *index) {
    if ( *index == 0 ) {
        return NULL;
    }
    *index -= 1;
    return self->array[*index];
}

void** ArrayLast(struct Array *const self, size_t *index) {
    if ( self->used == 0 ) {
        return NULL;
    }
    *index = self->used - 1;
    return self->array[*index];
}

void ArrayPush(struct Array *const self, void** element) {
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size 
    if ( self->used == self->size ) {
        self->size *= 2;
        self->array = reallocarray(self->array, self->size, sizeof(void**));
    }
    self->array[self->used++] = element;
}

void** ArrayPeek(struct Array *const self) {
    if ( self->used == 0 ) {
        return NULL;
    }
    return self->array[self->used - 1];
}

void** ArrayPop(struct Array *const self) {
    if ( self->used == 0 ) {
        return NULL;
    }
    return self->array[self->used-- - 1];
}

void ArrayForeach(struct Array *const self, ArrayIterFunc f) {
    for ( size_t i = 0; i<self->used; i++ ) {
        f(self->array[i]);
    }
}

void ArrayCopyTail(struct Array *const dst, struct Array *const src, size_t first) {
    size_t entries = src->used - first;
    memcpy(dst->array, &src->array[first], entries * sizeof(void**));
    dst->used = entries;
}

void ArrayFree(struct Array *const self) {
    if ( self ) {
        free(self->array);
        free(self);
    }
}
