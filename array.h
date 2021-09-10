/* selfrray.h */

#ifndef ARRAY_H
#define ARRAY_H

struct Array  {
  void** array;
  size_t used;
  size_t size;
  size_t savedPos;
};

typedef void (*ArrayIterFunc)(void**);

struct Array* ArrayNew(size_t initialSize);
int ArrayLen(struct Array *const self);
void ArraySwap(struct Array *const self, int i, int j);
void** ArrayGet(struct Array *const self, int pos);
void** ArrayFirst(struct Array *const self);
void** ArrayNext(struct Array *const self);
void** ArrayPrev(struct Array *const self);
void** ArrayLast(struct Array *const self);
void ArrayPush(struct Array *const self, void** element);
void** ArrayPeek(struct Array *const self);
void** ArrayPop(struct Array *const self);
void ArrayForeach(struct Array *self, ArrayIterFunc f);
void ArrayFree(struct Array *const self);

#endif
