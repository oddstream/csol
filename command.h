/* command.h */

#ifndef COMMAND_H
#define COMMAND_H

#include "baize.h"

typedef void (*CommandFunction)(struct Baize *const, void *param);

void StartCommandQueue(void);
void StopCommandQueue(void);

void PostCommand(CommandFunction bcf, void *param);
void PostUniqueCommand(CommandFunction bcf, void *param);
void ServiceCommandQueue(struct Baize *const baize);

#endif
