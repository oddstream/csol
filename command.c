/* command.c */

#include <stdio.h>

#include "baize.h"
#include "command.h"
#include "ui.h"

struct Command {
    // ISO C forbids conversion of object pointer to function pointer type [-Werror=pedantic]
    // so we hide our function pointer in a struct
    CommandFunction cf;
    void *param;    // NULL, or a pointer to a block of memory owned by widget
                    // i.e. the Widget frees this memory, no-one else
                    // currently used by TextWidget in variantDrawer: strdup(vname)
};

struct Array* CommandQueue = NULL;

void StartCommandQueue(void)
{
    CommandQueue = ArrayNew(8);
}

void StopCommandQueue(void)
{
    if (CommandQueue) {
        ArrayFree(CommandQueue);
    }
}

void PostCommand(CommandFunction cf, void *param)
{
    struct Command *c = calloc(1, sizeof(struct Command));
    if (c) {
        c->cf = cf;
        c->param = param;
        CommandQueue = ArrayPush(CommandQueue, c);
    }
}

void PostUniqueCommand(CommandFunction cf, void *param)
{
    _Bool found = 0;
    size_t index;
    for ( struct Command *c = ArrayFirst(CommandQueue, &index); c; c = ArrayNext(CommandQueue, &index) ) {
        if (c->cf == cf) {
            fprintf(stdout, "INFO: %s: command already in queue\n", __func__);
            found = 1;
            break;
        }
    }
    if (!found) {
        PostCommand(cf, param);
    }
}

void ServiceCommandQueue(struct Baize *const baize)
{
    if ( ArrayLen(CommandQueue) > 0 ) {
        // ISO C forbids conversion of object pointer to function pointer type [-Werror=pedantic]
        struct Command *c = ArrayGet(CommandQueue, 0);
        if (c) {
            if ( c->cf ) {
                UiHideDrawers(baize->ui);
                c->cf(baize, c->param);         // param is owned by Widget (most likely a TextWidget in variantDrawer)
            }
            ArrayDelete(CommandQueue, 0, free); // tell Array to free the Command object allocated in NewCommand()
        } else {
            ArrayDelete(CommandQueue, 0, NULL); // no Command object to free
        }
    }
}

