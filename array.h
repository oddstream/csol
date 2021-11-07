/* array.h */

#pragma once

#ifndef ARRAY_H
#define ARRAY_H

typedef void (*ArrayIterFunc)(void*);
typedef void (*ArrayFreeFunc)(void*);

struct Array* ArrayNew(size_t initialSize);
struct Array1 Array1New(void* element);
_Bool ArrayValid(struct Array *const self);
size_t ArrayLen(struct Array *const self);
size_t ArrayCap(struct Array *const self);
void ArrayDelete(struct Array *const self, size_t n, ArrayFreeFunc f);
void ArraySwap(struct Array *const self, int i, int j);
void* ArrayGet(struct Array *const self, size_t pos);
void ArrayPut(struct Array *const self, size_t pos, void* element);
_Bool ArrayIndexOf(struct Array *const self, const void * element, size_t *index);
void* ArrayFirst(struct Array *const self, size_t *index);
void* ArrayNext(struct Array *const self, size_t *index);
void* ArrayPrev(struct Array *const self, size_t *index);
void* ArrayLast(struct Array *const self, size_t *index);
struct Array* ArrayPush(struct Array *self, void* element) __attribute__ ((warn_unused_result));
void* ArrayPeek(struct Array *const self);
void* ArrayPop(struct Array *const self);
void ArrayForeach(struct Array *self, ArrayIterFunc f);
void ArrayCopyTail(struct Array *const dst, struct Array *const src, size_t first);
struct Array* ArrayClone(struct Array *const self);
void ArrayReset(struct Array *const dst);
void ArrayFree(struct Array *const self);

struct ArrayIterator {
    struct Array *a;
    size_t i;
};

struct ArrayIterator ArrayIterator(struct Array *const self);
void *ArrayMoveNext(struct ArrayIterator* self);

// this can't be opaque, because it's allocated on the stack
// so it's defined in this header
struct Array1 {
    unsigned magic;
    size_t used;
    size_t size;
    void* data[1];
};

#endif
