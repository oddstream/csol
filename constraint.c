/* constraint.c */

#include <stdlib.h>

#include "array.h"
#include "baize.h"
#include "constraint.h"
#include "trace.h"

_Bool CanTailBeMoved(struct Array *const tail)
{
    struct Card *const c0 = ArrayGet(tail, 0);

    // BaizeTouchStop() checks if any tail cards are prone

    struct Pile *const pile = CardOwner(c0);
    struct Baize *const baize = PileOwner(pile);

    const char *strerr = baize->exiface->TailMoveError(tail);
    if (strerr) {
        BaizeSetError(baize, strerr);
        return 0;
    }

    return 1;
}

_Bool CanTailBeAppended(struct Pile *const pile, struct Array *const tail)
{
    if (!PileValid(pile)) {
        CSOL_ERROR("%s", "invalid pile");
        return 0;
    }
    if (!tail) {
        CSOL_ERROR("%s", "invalid tail");
        return 0;
    }

    struct Baize *const baize = PileOwner(pile);
    const char *strerr = baize->exiface->TailAppendError(pile, tail);
    if (strerr) {
        BaizeSetError(baize, strerr);
        return 0;
    }

    return 1;
}

_Bool IsPileConformant(struct Pile *const pile)
{
    if (!PileValid(pile)) {
        CSOL_ERROR("%s", "invalid pile");
        return 0;
    }

    struct Baize *const baize = PileOwner(pile);
    const char *strerr = baize->exiface->PileConformantError(pile);
    if (strerr) {
        BaizeSetError(baize, strerr);
        return 0;
    }

    return 1;
}
