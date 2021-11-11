/* scrunch.c */

#include <stdio.h>
#include <stdlib.h>

#include "array.h"
#include "baize.h"
#include "pile.h"
#include "scrunch.h"

void BaizeCalculateScrunchLimits(struct Baize *const baize, const int windowWidth, const int windowHeight)
{
    extern float cardWidth, cardHeight;

    size_t pi1, pi2;
    for ( struct Pile *p1 = ArrayFirst(baize->piles, &pi1); p1; p1 = ArrayNext(baize->piles, &pi1) ) {

        /*
            K&R style switch formatting, see P59 if you don't believe me
            (ok, thay may have done it like this to fit in the book margins
        */
        switch (p1->fanType) {
        case FAN_NONE:
        case FAN_DOWN3:
        case FAN_LEFT3:
        case FAN_RIGHT3:
            p1->scrunchLimit = -1.0f;   // by default, do not scrunch
            break;
        case FAN_DOWN:
            p1->scrunchLimit = windowHeight - cardHeight;
            break;
        case FAN_LEFT:
            p1->scrunchLimit = 0.0f;
            break;
        case FAN_RIGHT:
            p1->scrunchLimit = windowWidth - cardWidth;
            break;
        default:
            break;
        }

        p1->fanFactor = p1->defaultFanFactor;

        for ( struct Pile *p2 = ArrayFirst(baize->piles, &pi2); p2; p2 = ArrayNext(baize->piles, &pi2) ) {
            if (p2 == p1) {
                continue;
            }
            switch (p1->fanType) {
            case FAN_NONE:
            case FAN_DOWN3:
            case FAN_LEFT3:
            case FAN_RIGHT3:
                break;
            case FAN_DOWN:
                if (p1->slot.x == p2->slot.x && p2->slot.y > p1->slot.y) {
                    // fprintf(stdout,
                    //     "INFO: %s: found %s pile at %.0f,%.0f "
                    //     "that is above %s pile at %.0f,%.0f"
                    //     "\n",
                    // __func__,
                    // p1->category, p1->slot.x, p1->slot.y,
                    // p2->category, p2->slot.x, p2->slot.y);
                    p1->scrunchLimit = p2->pos.y - cardHeight;
                }
                break;
            case FAN_LEFT:
                if (p1->slot.y == p2->slot.y && p2->slot.x < p1->slot.x) {
                    p1->scrunchLimit = p2->pos.x + cardWidth;
                }
                break;
            case FAN_RIGHT:
                if (p1->slot.y == p2->slot.y && p2->slot.x > p1->slot.x) {
                    p1->scrunchLimit = p2->pos.x - cardWidth;
                }
                break;
            default:
                break;
            }
        }

    }
}

_Bool ScrunchOverflow(struct Pile *const pile, Vector2 newpos)
{
    _Bool result;
    switch (pile->fanType) {
    case FAN_NONE:
    case FAN_DOWN3:
    case FAN_LEFT3:
    case FAN_RIGHT3:
        result = false;
        break;
    case FAN_DOWN:
        result = (newpos.y > pile->scrunchLimit);
        break;
    case FAN_LEFT:
        result = (newpos.x < pile->scrunchLimit);
        break;
    case FAN_RIGHT:
        result = (newpos.x > pile->scrunchLimit);
        break;
    default:
        break;
    }
    return result;
}

void ScrunchPile(struct Pile *const pile)
{
    extern float cardHeight;

    if (pile->scrunchLimit<0.0f) {
        return;
    }

    // Vector2 pos;
    size_t len;

    len = ArrayLen(pile->cards);
    if (len < 2) {
        // zero or one card, no scrunching required, ever
        return;
    }
    // two cards? incoming card will have been placed at slot 2
    struct Card *c = PilePeekCard(pile);
    if (ScrunchOverflow(pile, c->pos)) {
        // fprintf(stdout, "INFO: %s: scrunching %s by %.0f\n", __func__, pile->category, (pile->scrunchLimit - c->pos.y));
        // pile->fanFactor += (pile->scrunchLimit - c->pos.y);
        pile->fanFactor += 1.0f;
        if (pile->fanFactor > MAX_FAN_FACTOR) {
            pile->fanFactor = MAX_FAN_FACTOR;
        }
        PileRefan(pile);
    }
}

void ScrunchPiles(struct Baize *const baize)
{
    ArrayForeach(baize->piles, (ArrayIterFunc)ScrunchPile);
}
