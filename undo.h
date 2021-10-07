/* undo.h */

#ifndef UNDO_H
#define UNDO_H

struct Array* UndoStackNew(void);
void UndoStackFree(struct Array *stack);
void BaizeUndoPush(struct Baize *const self);
struct Array* BaizeUndoPop(struct Baize *const self);
void BaizeUpdateFromSnapshot(struct Baize *const self, struct Array *savedPiles);
void BaizeSavePositionCommand(struct Baize *const self, void* param);
void BaizeLoadPositionCommand(struct Baize *const self, void* param);
void BaizeRestartDealCommand(struct Baize *const self, void* param);
void BaizeUndoCommand(struct Baize *const, void* param);

#endif
