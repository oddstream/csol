/* command.c */

#include "baize.h"
#include "command.h"
#include "ui.h"

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

void NewCommand(CommandFunction cf, void* param)
{
    struct Command *c = calloc(1, sizeof(struct Command));
    if (c) {
        c->cf = cf;
        c->param = param;
        CommandQueue = ArrayPush(CommandQueue, c);
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

