/* command.h */

#ifndef COMMAND_H
#define COMMAND_H

#include "baize.h"

struct Array* CommandQueue;

typedef void (*CommandFunction)(struct Baize *const, void* param);

struct Command {
    // ISO C forbids conversion of object pointer to function pointer type [-Werror=pedantic]
    // so we hide our function pointer in a struct
    CommandFunction cf;
    void *param;    // NULL, or a pointer to a block of memory owned by widget
                    // i.e. the Widget frees this memory, no-one else
                    // currently used by TextWidget in variantDrawer: strdup(vname)
};

void StartCommandQueue(void);
void StopCommandQueue(void);

void NewCommand(CommandFunction bcf, void *param);
void ServiceCommandQueue(struct Baize *const baize);

#endif
