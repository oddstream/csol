/* thisrray.h */

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
int ArrayLen(struct Array* this);
void ArraySwap(struct Array* self, int i, int j);
void** ArrayGet(struct Array* this, int pos);
void** ArrayFirst(struct Array* this);
void** ArrayNext(struct Array* this);
void** ArrayPrev(struct Array* this);
void** ArrayLast(struct Array* this);
void ArrayPush(struct Array* this, void** element);
void** ArrayPeek(struct Array* this);
void** ArrayPop(struct Array* this);
void ArrayForeach(struct Array *self, ArrayIterFunc f);
void ArrayFree(struct Array *a);

#endif
