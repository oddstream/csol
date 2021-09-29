/* array.c */

#include <stdlib.h>
#include <string.h>

#include "array.h"

struct Array* ArrayNew(size_t initialSize)
{
    struct Array* self = calloc(1, sizeof(struct Array));
    self->used = 0;
    self->size = initialSize;
    self->data = calloc(initialSize, sizeof(void*));
    return self;
}

size_t ArrayLen(struct Array *const self)
{
    return self->used;
}

size_t ArrayCap(struct Array *const self)
{
    return self->size;
}

void ArrayDeleteFirst(struct Array *const self, ArrayFreeFunc f)
{
    if ( self->used == 0 ) {
        return;
    }
    void *item = self->data[0];
    if ( f ) {
        f(item);
    }
    for ( size_t i = 1; i<self->used; i++ ) {
        self->data[i-1] = self->data[i];
    }
    self->used -= 1;
}

void ArraySwap(struct Array *const self, int i, int j)
{
    void* tmp = self->data[i];
    self->data[i] = self->data[j];
    self->data[j] = tmp;
}

void* ArrayGet(struct Array *const self, int pos)
{
    return self->data[pos];
}

void* ArrayFirst(struct Array *const self, size_t *index)
{
    if ( self->used == 0 ) {
        return NULL;
    }
    *index = 0;
    return self->data[0];
}

void* ArrayNext(struct Array *const self, size_t *index)
{
    *index += 1;
    if ( *index >= self->used ) {
        return NULL;
    }
    return self->data[*index];
}

void* ArrayPrev(struct Array *const self, size_t *index)
{
    if ( *index == 0 ) {
        return NULL;
    }
    *index -= 1;
    return self->data[*index];
}

void* ArrayLast(struct Array *const self, size_t *index)
{
    if ( self->used == 0 ) {
        return NULL;
    }
    *index = self->used - 1;
    return self->data[*index];
}

void ArrayPush(struct Array *const self, void* element)
{
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size 
    if ( self->used == self->size ) {
        self->size *= 2;
        self->data = reallocarray(self->data, self->size, sizeof(void*));
    }
    self->data[self->used++] = element;
}

void* ArrayPeek(struct Array *const self)
{
    if ( self->used == 0 ) {
        return NULL;
    }
    return self->data[self->used - 1];
}

void* ArrayPop(struct Array *const self)
{
    if ( self->used == 0 ) {
        return NULL;
    }
    return self->data[self->used-- - 1];
}

void ArrayForeach(struct Array *const self, ArrayIterFunc f)
{
    for ( size_t i = 0; i<self->used; i++ ) {
        f(self->data[i]);
    }
}

void ArrayCopyTail(struct Array *const dst, struct Array *const src, size_t first)
{
    size_t entries = src->used - first;
    memcpy(dst->data, &src->data[first], entries * sizeof(void*));
    dst->used = entries;
}

struct Array* ArrayClone(struct Array *const self)
{
    struct Array *clone = ArrayNew(self->size);
    memcpy(clone->data, self->data, sizeof(void*) * self->size);
    clone->size = self->size;
    clone->used = self->used;
    return clone;
}

void ArrayReset(struct Array *const self)
{
    self->used = 0;
}

void ArrayFree(struct Array *const self)
{
    if ( self ) {
        free(self->data);
        free(self);
    }
}
