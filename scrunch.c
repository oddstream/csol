/* scrunch.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "array.h"
#include "baize.h"
#include "pile.h"
#include "scrunch.h"

void BaizeCalculateScrunchDims(struct Baize *const baize, const int windowWidth, const int windowHeight)
{
    struct Pack *pack = baize->pack;

    size_t pi1, pi2;
    for ( struct Pile *p1 = ArrayFirst(baize->piles, &pi1); p1; p1 = ArrayNext(baize->piles, &pi1) ) {

        /*
            K&R style switch formatting, see P59 if you don't believe me
            (ok, thay may have done it like this to fit in the book margins
        */
        p1->scrunchDims = (Vector2){.x=pack->width, .y=pack->height};
        switch (p1->fanType) {
        case FAN_NONE:
        case FAN_DOWN3:
        case FAN_LEFT3:
        case FAN_RIGHT3:
            break;
        case FAN_DOWN:
            // baize->dragOffset is always -ve
            p1->scrunchDims.y = windowHeight - p1->pos.y + fabsf(baize->dragOffset.y);
            break;
        case FAN_LEFT:
            p1->scrunchDims.x = p1->pos.x;
            break;
        case FAN_RIGHT:
            // baize->dragOffset is always -ve
            p1->scrunchDims.x = windowWidth - p1->pos.x + fabsf(baize->dragOffset.x);
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
                    p1->scrunchDims.y = p2->pos.y - p1->pos.y;
                }
                break;
            case FAN_LEFT:
                if (p1->slot.y == p2->slot.y && p2->slot.x < p1->slot.x) {
                    p1->scrunchDims.x = p1->pos.x - p2->pos.x;
                }
                break;
            case FAN_RIGHT:
                if (p1->slot.y == p2->slot.y && p2->slot.x > p1->slot.x) {
                    p1->scrunchDims.x = p2->pos.x - p1->pos.x;
                }
                break;
            default:
                break;
            }
        }

    }
}

// calculate the width and heigh this pile would be if it had a specified fan factor
static Vector2 calcPileDimensions(struct Pile *const pile, float fanFactor)
{
    struct Baize *baize = PileOwner(pile);
    struct Pack *pack = baize->pack;

    Vector2 dims = (Vector2){.x=pack->width, .y=pack->height};
    size_t len = ArrayLen(pile->cards);
    if (len<2) {
        return dims;
    }

    switch (pile->fanType) {
    case FAN_NONE:
        // well, that was easy
        break;
    case FAN_DOWN3:
        switch (len) {
        case 0:
        case 1:
            break;
        case 2:
            dims.y += (pack->height / CARD_FACE_FAN_FACTOR_V);
            break;
        default:
            dims.y += (pack->height / CARD_FACE_FAN_FACTOR_V) * 2;
            break;
        }
        break;
    case FAN_LEFT3:
    case FAN_RIGHT3:
        switch (len) {
        case 0:
        case 1:
            break;
        case 2:
            dims.x += (pack->width / CARD_FACE_FAN_FACTOR_H);
            break;
        default:
            dims.x += (pack->width / CARD_FACE_FAN_FACTOR_H) * 2;
            break;
        }
        break;
    case FAN_DOWN:
        // dims.y = 0.0f;
        for ( size_t i=0; i<len-1; i++ ) {
            struct Card *c = ArrayGet(pile->cards, i);
            dims.y += (c->prone) ? pack->height / CARD_BACK_FAN_FACTOR : pack->height / fanFactor;
        }
        // dims.y += pack->height;
        break;
    case FAN_LEFT:
    case FAN_RIGHT:
        // dims.x = 0.0f;
        for ( size_t i=0; i<len-1; i++ ) {
            struct Card *c = ArrayGet(pile->cards, i);
            dims.x += (c->prone) ? pack->width / CARD_BACK_FAN_FACTOR : pack->width / fanFactor;
        }
        // dims.x += pack->width;
        break;
    default:
        break;
    }

    return dims;
}

// check the scrunch of this pile and adjust if necessary
void ScrunchPile(struct Pile *const pile)
{
    struct Baize *baize = PileOwner(pile);
    struct Pack *pack = baize->pack;

    if (!(pile->scrunchDims.x > pack->width || pile->scrunchDims.y > pack->height)) {
        return; // disregard waste-style piles and those that do not fan
    }

    // Vector2 pos;
    size_t len = ArrayLen(pile->cards);
    if (len < 2) {
        // zero or one card, no scrunching required, ever
        return;
    }

    float fanFactor;
    for ( fanFactor = pile->defaultFanFactor; fanFactor < MAX_FAN_FACTOR; fanFactor += 0.5f ) {
        Vector2 dims = calcPileDimensions(pile, fanFactor);
        if (pile->fanType == FAN_DOWN) {
            if (dims.y < pile->scrunchDims.y) {
                break;
            }
        } else {
            if (dims.x < pile->scrunchDims.x) {
                break;
            }
        }
        // fprintf(stdout, "going round again\n");
    }
    if (fanFactor != pile->fanFactor) {
        pile->fanFactor = fanFactor;
        PileRefan(pile);
    }
}

void ScrunchPiles(struct Baize *const baize)
{
    ArrayForeach(baize->piles, (ArrayIterFunc)ScrunchPile);
}

void ScrunchDrawDebug(struct Pile *const pile)
{
    struct Baize *baize = PileOwner(pile);
    struct Pack *pack = baize->pack;
    
    Rectangle r = PileScreenRect(pile);

    char buff[64];
    sprintf(buff, "dff=%.0f ff=%.0f", pile->defaultFanFactor, pile->fanFactor);
    DrawText(buff, r.x + 10, r.y + 100, pack->pileFontSize / 2.0f, GREEN);

    r.width = pile->scrunchDims.x;
    r.height = pile->scrunchDims.y;
    DrawRectangleRoundedLines(r, pack->roundness, 9, 1.0, RED);

    Vector2 dims = calcPileDimensions(pile, pile->fanFactor);
    r.width = dims.x;
    r.height = dims.y;
    DrawRectangleRoundedLines(r, pack->roundness, 9, 1.0, GREEN);
}
