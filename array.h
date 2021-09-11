/* selfrray.h */

#ifndef ARRAY_H
#define ARRAY_H

struct Array  {
  void** array;
  size_t used;
  size_t size;
};

typedef void (*ArrayIterFunc)(void*);

struct Array* ArrayNew(size_t initialSize);
size_t ArrayLen(struct Array *const self);
size_t ArrayCap(struct Array *const self);
void ArraySwap(struct Array *const self, int i, int j);
void** ArrayGet(struct Array *const self, int pos);
void** ArrayFirst(struct Array *const self, size_t *index);
void** ArrayNext(struct Array *const self, size_t *index);
void** ArrayPrev(struct Array *const self, size_t *index);
void** ArrayLast(struct Array *const self, size_t *index);
void ArrayPush(struct Array *const self, void** element);
void** ArrayPeek(struct Array *const self);
void** ArrayPop(struct Array *const self);
void ArrayForeach(struct Array *self, ArrayIterFunc f);
void ArrayCopyTail(struct Array *const dst, struct Array *const src, size_t first);
void ArrayFree(struct Array *const self);

#endif
