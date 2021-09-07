/* array.h */

#ifndef ARRAY_H
#define ARRAY_H

struct Array  {
  void** array;
  size_t used;
  size_t size;
};

typedef void (*ArrayIterFunc)(void**);

struct Array ArrayNew(size_t initialSize);
int ArrayLen(struct Array* a);
void** ArrayGet(struct Array* a, int pos);
void** ArrayFirst(struct Array* a, int* savedPos);
void** ArrayNext(struct Array* a, int* savedPos);
void** ArrayPrev(struct Array* a, int* savedPos);
void** ArrayLast(struct Array* a, int* savedPos);
void ArrayPush(struct Array* a, void** element);
void** ArrayPeek(struct Array* a);
void** ArrayPop(struct Array* a);
void ArrayForeach(struct Array *self, ArrayIterFunc f);
void ArrayFree(struct Array *a);

#endif
