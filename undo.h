/* undo.h */

#ifndef UNDO_H
#define UNDO_H

struct Snapshot {
    int recycles, faccept, taccept;
    struct Array *savedPiles;
};

struct Array* UndoStackNew(void);
void UndoStackFree(struct Array *stack);
void BaizeUndoPush(struct Baize *const self);
struct Snapshot* BaizeUndoPop(struct Baize *const self);
void BaizeUpdateFromSnapshot(struct Baize *const self, struct Snapshot *s);
void BaizeSavePositionCommand(struct Baize *const self, void* param);
void BaizeLoadPositionCommand(struct Baize *const self, void* param);
void BaizeRestartDealCommand(struct Baize *const self, void* param);
void BaizeUndo0(struct Baize *const self);
void BaizeUndo(struct Baize *const self);
void BaizeUndoCommand(struct Baize *const, void* param);

void BaizeSaveUndoToFile(struct Baize *const self);
struct Array* LoadUndoFromFile(char *variantName);

#endif
