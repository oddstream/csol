/* exiface.c */

#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "card.h"
#include "exiface.h"
#include "trace.h"

struct knownInterface {
    char name[32];
    struct ExecutionInterface* (*Ctor)(void);
};

static struct knownInterface knownInterfaces[3] = {
    { "Fallback", GetFallbackInterface  },  // fallback comes first
    { "Freecell", GetFreecellInterface  },
    { "Klondike", GetKlondikeInterface  },
};

// TODO baize->variantName is probably already set so retire name parameter
struct ExecutionInterface* GetInterface(struct Baize *const baize)
{
    struct ExecutionInterface *self = (void*)0;

    // try to find a Lua scripted game first, so inbuilt games can be overridden
    char fname[128];    snprintf(fname, 127, "variants/%s.lua", baize->variantName);
    if ( luaL_loadfile(baize->L, fname) || lua_pcall(baize->L, 0, 0, 0) ) {
        CSOL_ERROR("Cannot find script for game '%s', error: %s", baize->variantName, lua_tostring(baize->L, -1));
        lua_pop(baize->L, 1);

        for ( size_t i=0; i<sizeof(knownInterfaces)/sizeof(struct knownInterface); i++ ) {
            if (strcmp(knownInterfaces[i].name, baize->variantName) == 0) {
                self = knownInterfaces[i].Ctor();
                break;
            }
        }
    } else {
        CSOL_INFO("Loaded '%s'", fname);
        // we found a Lua script, and it loads, so use the csol-Lua bridge interface
        self = GetMoonInterface();
    }

    if (!self) {
        CSOL_INFO("Cannot find rules for game '%s', using fallback", baize->variantName);
        self = knownInterfaces[0].Ctor();
    }

    return self;
}

// useful generic game library of functions

static int CardColor(struct Card *const c)
{
    return (CardSuit(c) == DIAMOND || CardSuit(c) == HEART) ? 1 : 0;
}

const char* CardCompare_Up(struct Card *const c1, struct Card *const c2)
{
    if (CardOrdinal(c1) + 1 != CardOrdinal(c2))
        return "Cards must be in ascending order";
    return (void*)0;
}

const char* CardCompare_Down(struct Card *const c1, struct Card *const c2)
{
    if (CardOrdinal(c1) != CardOrdinal(c2) + 1)
        return "Cards must be in descending order";
    return (void*)0;
}

const char* CardCompare_DownAltColor(struct Card *const c1, struct Card *const c2)
{
    if (CardColor(c1) == CardColor(c2))
        return "Cards must be in alternating colors";
    return CardCompare_Down(c1, c2);
}

const char* CardCompare_DownAltColorWrap(struct Card *const c1, struct Card *const c2)
{
    if (CardColor(c1) == CardColor(c2))
        return "Cards must be in alternating colors";
    if (CardOrdinal(c1) == 1 && CardOrdinal(c2) == 13)
        return (void*)0;   // King on Ace
    return CardCompare_Down(c1, c2);
}

const char* CardCompare_UpAltColor(struct Card *const c1, struct Card *const c2)
{
    if (CardColor(c1) == CardColor(c2))
        return "Cards must be in alternating colors";
    return CardCompare_Up(c1, c2);
}

const char* CardCompare_UpSuit(struct Card *const c1, struct Card *const c2)
{
    if (CardSuit(c1) != CardSuit(c2))
        return "Cards must be the same suit";
    return CardCompare_Up(c1, c2);
}

const char* CardCompare_DownSuit(struct Card *const c1, struct Card *const c2)
{
    if (CardSuit(c1) != CardSuit(c2))
        return "Cards must be the same suit";
    return CardCompare_Down(c1, c2);
}

const char* CardCompare_UpSuitWrap(struct Card *const c1, struct Card *const c2)
{
    if (CardSuit(c1) != CardSuit(c2))
        return "Cards must be the same suit";
    if (CardOrdinal(c1) == 13 && CardOrdinal(c2) == 1)
        return (void*)0;
    else if (CardOrdinal(c1) == CardOrdinal(c2) - 1)
        return (void*)0;
    else
        return "Cards must go up in rank (Kings on Aces allowed)";
}

const char* CardCompare_DownSuitWrap(struct Card *const c1, struct Card *const c2)
{
    if (CardSuit(c1) != CardSuit(c2))
        return "Cards must be the same suit";
    if (CardOrdinal(c1) == 1 && CardOrdinal(c2) == 13)
        return (void*)0;
    else if (CardOrdinal(c1) - 1 == CardOrdinal(c2))
        return (void*)0;
    else
        return "Cards must go down in rank (Kings on Aces allowed)";
}
