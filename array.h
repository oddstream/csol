/* array.h */

#ifndef ARRAY_H
#define ARRAY_H

struct Array  {
  void** array;
  size_t used;
  size_t size;
};

struct Array ArrayNew(size_t initialSize);
int ArrayLen(struct Array* a);
void ArrayPush(struct Array* a, void** element);
void** ArrayPeek(struct Array* a);
void** ArrayPop(struct Array* a);
void ArrayFree(struct Array *a);

#endif
