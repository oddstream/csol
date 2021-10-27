/* array.h */

#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_MAGIC (0xfeedface)

struct Array  {
    unsigned magic;
    size_t used;
    size_t size;
    void* data[];  // void** so we can dereference it (eg use data[i])
    // using data[] (an 'incompete type'), instead of void**, then self changes when we realloc
    // which means ArrayPush() &c may change self
    // so we have to use eg piles->array = ArrayPush(piles->array, p)
};

struct Array1 {
    unsigned magic;
    size_t used;
    size_t size;
    void* data[1];
};

typedef void (*ArrayIterFunc)(void*);
typedef void (*ArrayFreeFunc)(void*);

struct Array* ArrayNew(size_t initialSize);
_Bool ArrayValid(struct Array *const self);
size_t ArrayLen(struct Array *const self);
size_t ArrayCap(struct Array *const self);
void ArrayDelete(struct Array *const self, size_t n, ArrayFreeFunc f);
void ArraySwap(struct Array *const self, int i, int j);
void* ArrayGet(struct Array *const self, int pos);
_Bool ArrayIndexOf(struct Array *const self, void *element, size_t *index);
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

#endif
