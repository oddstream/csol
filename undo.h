/* undo.h */

#ifndef UNDO_H
#define UNDO_H

struct Array* SnapshotNew(struct Baize *const self);
void SnapshotFree(struct Array* savedPiles);
struct Array* UndoStackNew(void);
void UndoStackFree(struct Array *stack);
void UndoPush(struct Baize *const self);
struct Array* UndoPop(struct Baize *const self);

#endif
