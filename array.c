/* array.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "array.h"

#define ARRAY_MAGIC (0xfeedface)

// define the struct Array here so it's opaque
struct Array {
    unsigned magic;
    size_t used;
    size_t size;
    void* data[];  // void** so we can dereference it (eg use data[i])
    // using data[] (an 'incompete type'), instead of void**, then self changes when we realloc
    // which means ArrayPush() &c may change self
    // so we have to use eg piles->array = ArrayPush(piles->array, p)
};

struct Array* ArrayNew(size_t initialSize)
{
    // struct Array* self = calloc(1, sizeof(struct Array));
    // self->used = 0;
    // self->size = initialSize;
    // self->data = calloc(initialSize, sizeof(void*));
    struct Array *self = calloc(1, sizeof(struct Array) + sizeof(void*[initialSize]));
    if (self) {
        self->magic = ARRAY_MAGIC;
        self->used = 0;
        self->size = initialSize;
    }
    return self;
}

struct Array1 Array1New(void* element)
{
    return (struct Array1){.magic=ARRAY_MAGIC, .used=1, .size=1, .data[0]=element};
}

/*
    stack protector not protecting local variables
    remove -Wstack-protector -Wstack-usage=1000000 from Makefile
struct Array* ArrayNewTmp1(void *element)
{
    struct Array* self = alloca(sizeof(struct Array) + sizeof(void*[1]));
    if (self) {
        self->used = 1;
        self->size = 1;
        self->data[0] = element;
    }
    return self;
}
*/

_Bool ArrayValid(struct Array *const self)
{
    if (!self) {
        fprintf(stderr, "ERROR: %s: null\n", __func__);
        return 0;
    }
    if (self->magic != ARRAY_MAGIC) {
        fprintf(stderr, "ERROR: %s: bad magic, expected %x, got %x\n", __func__, ARRAY_MAGIC, self->magic);
        return 0;
    }
    if (self->used > self->size) {
        fprintf(stderr, "ERROR: %s: used=%lu, size=%lu\n", __func__, self->used, self->size);
        return 0;
    }
    return 1;
}

size_t ArrayLen(struct Array *const self)
{
    return self->used;
}

size_t ArrayCap(struct Array *const self)
{
    return self->size;
}

void ArrayDelete(struct Array *const self, size_t n, ArrayFreeFunc ff)
{
    if (self->used < n) {
        return;
    }
    void *item = self->data[n];
    if (ff) {
        ff(item);
    }
    for ( ++n; n<self->used; n++ ) {
        self->data[n-1] = self->data[n];
    }
    self->used -= 1;
}

void ArraySwap(struct Array *const self, int i, int j)
{
    void* tmp = self->data[i];
    self->data[i] = self->data[j];
    self->data[j] = tmp;
}

void* ArrayGet(struct Array *const self, size_t pos)
{
    if (pos < self->used) {
        return self->data[pos];
    } else {
        fprintf(stderr, "WARNING: %s: index %lu exceeds used %lu\n", __func__, pos, self->used);
        return NULL;
    }
}

// replace the element at pos - does not expand array
void ArrayPut(struct Array *const self, size_t pos, void* element)
{
    if (pos < self->used) {
        self->data[pos] = element;
    } else {
        fprintf(stderr, "WARNING: %s: index %lu exceeds used %lu\n", __func__, pos, self->used);
    }
}

// return the index of element in out parameter, and true/false if element was found
_Bool ArrayIndexOf(struct Array *const self, const void *element, size_t *index)
{
    for ( size_t i=0; i<self->used; i++ ) {
        if (self->data[i] == element) {
            if (index) *index = i;
            return 1;
        }
    }
    return 0;
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

struct Array* ArrayPush(struct Array *self, void* element)
{
    // a->used is the number of used entries,
    // because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size
    if ( self->used == self->size ) {
        struct Array *new = calloc(1, sizeof(struct Array) + sizeof(void*[self->size * 2]));
        if (new) {
            new->size = self->size * 2;
            new->used = self->used;
            memcpy(new->data, self->data, self->used * sizeof(void*));
            free(self);
            self = new;
        }
    //     self->size *= 2;
    //     self->data = reallocarray(self->data, self->size, sizeof(void*));
    }

    self->data[self->used++] = element;
    return self;
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
        self->magic = 0;
        // free(self->data);
        free(self);
    }
}

struct ArrayIterator ArrayIterator(struct Array *const a)
{
    return (struct ArrayIterator){.a = a, .i = 0};
}

void* ArrayMoveNext(struct ArrayIterator *self)
{
    if (self->a->used > self->i) {
        return self->a->data[(self->i)++];
    }
    return NULL;
}