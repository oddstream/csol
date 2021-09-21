/* undo.h */

#ifndef UNDO_H
#define UNDO_H

struct Array* SnapshotNew(struct Baize *const self);
void SnapshotFree(struct Array* savedPiles);
struct Array* UndoStackNew(void);
void UndoStackFree(struct Array *stack);
void BaizeUndoPush(struct Baize *const self);
struct Array* BaizeUndoPop(struct Baize *const self);
void PileUpdateFromCardArray(struct Pile *const self, struct Array *cards);
void BaizeUpdateFromSnapshot(struct Baize *const self, struct Array *savedPiles);
void BaizeUndoCommand(struct Baize *const);

#endif
