/* undo.h */

#ifndef UNDO_H
#define UNDO_H

struct Snapshot {
    int recycles;
    size_t bookmark;
    struct Array *savedPiles;
};

struct Array* UndoStackNew(void);
void UndoStackFree(struct Array *stack);
void BaizeUndoPush(struct Baize *const self);
void BaizeUpdateFromSnapshot(struct Baize *const self, struct Snapshot *s);
void BaizeSavePositionCommand(struct Baize *const self, void* param);
void BaizeLoadPositionCommand(struct Baize *const self, void* param);
void BaizeRestartDealCommand(struct Baize *const self, void* param);
void BaizeUndo0(struct Baize *const self);
void BaizeUndoCommand(struct Baize *const, void* param);

#if 0
void BaizeSaveUndoToFile(struct Baize *const self);
struct Array* LoadUndoFromFile(char *variantName);
#endif

#endif
