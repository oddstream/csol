# csol

## Next

- [ ] _Generic
- [ ] a savedPile is (`Pile` subtype members) and an `Array` of `Card*`, (in memory only for now, serialize later)
- [ ] a savedBaize is a header (checksum, state) and an `Array` of `savedPile`
- [ ] undo stack an `Array` of `savedBaize`

- [ ] don't like passing Baize->L around, now have ->owner
- [ ] state
- [ ] MoveCard(STOCK, pile, prone) YAGNI
- [ ] CanAcceptTail should return an error string for toasting later
- [ ] baize dragging
- [ ] draw recycle symbol
- [ ] why does LoadFont cause a segmentation fault?
- [ ] toast update/draw (error return from some functions could be a string)
- [ ] CardSetPos() problem
- [ ] title bar
- [ ] status bar
- [x] CardToString, LOGCARD
- [x] CardId (easy to serialize)
- [x] Klondike (CardTapped deal from Stock to Waste, PileTapped recycle from Waste to Stock)
- [x] The empty tableau can only accept a 13, not a 11 (dragging KQJ to empty tab)
- [x] Waste fanning
- [x] Card flipping
- [x] conformant functions in Lua? Register a conformant function from inside Lua Build RegisterConformantFunction("Tableau", TableauFn) and then call it from C with two Card args cPrev cNext, each arg is a table with {ord=3,suit=S,prone=false}
- [x] can remove a lot of Pile functions from the vtables
- [x] move csol.settings.lua into variant.lua
- [x] Baize.LargestIntersection(baize, card)
- [x] Pile.MoveCards(dst pile, src card)
- [x] move tail after drag
- [ ] compact pile/card format for undo
        shorthand 2-char with lowercase suit for face down, uppercase suit for face up "2C3C4C4c"
        also need pilenumber, accept, recycles
- [x] drag a tail
- [x] pyle subtypes
- deal cards DealUp(p, n)/DealDown(p, n)/DealCard(p, "AC")
- build only task
- minimal Makefile

## Conventions

YAGNI, minimalist

No typedefs on structs or enums.

Functions names are in PascalCase, ObjectVerb.

Lua uses `self`, so do we. `this` is a reserved word in C++.

Hashed string heap for quick string comparisons?

Club, Diamond, Heart, Spade - always in that order (alphabetical).

Ordinal, Suit - always in that order (alphabetical, and how they are shown on cards).

Position (Vector2) functions and members refer to the Baize position of the Card/Pile, unless they have Screen in their name. All functions have Get and Set in their name, no matter how unfashionable.

Use "AC", "2H", "XD", "KS" shorthand for cards.

Any card or cards can be dragged anywhere, anytime. If the move breaks the rules, csol puts the cards back. Implies no need for card shaking.

Some behaviour is baked into each pyle subtype (eg cells only hold one card, foundations can only accept one card at a time, accept rules (which are set from Lua), Stock and Waste behaviour), everything else is configured by *variant.lua*.

## Makefile

`/usr/local/lib/libraylib.a`

`/usr/local/include/raylib.h`
`/usr/local/include/physac.h`
`/usr/local/include/raymath.h`
`/usr/local/include/rlgl.h`

`/home/gilbert/lua-5.4.3/src/liblua.a`

`/home/gilbert/lua-5.4.3/src/lua.h`

Edit `c_cpp_properties.json` to add `/home/gilbet/lua-5.4.3/src` to `includePath`.

## Variant

Command-driven config files, one per variant, stored in a directory with filename = variant name. Tricky if compiled with emscripten and running in a browser.

Programming in Lua 4th Edition uses 5.3

Lastest version is 5.4.3

Maybe edit `luaconf.h` to use int and float the same size as raylib (which certainly uses float a lot).

```lua
-- Freecell

-- variant globals
name = "Freecell"
description = "Popular game invented by Paul Alfille, unusual because almost all deals are winnable."
wikipedia = "https://en.wikipedia.org/wiki/FreeCell"
packs = 1 -- for building cardLibrary, defaults to 1

function BuildVariant(name)
    local stock, p

    stock = CreatePile("Stock", -2, -2, "None", {0,0,0})

    p = PileCreate("Foundation", 4, 0, "None", {21,0,0})
    -- is p userdata
    PileSetAccept(p, "AC")

    p = CreatePile("Tableau", 0, 1, "Down", {42,42,1})
    PileDeal(p, "UUUUUUU")

end
```

## Fonts

See [Parrots font drawing] for use of ttf font.

Can use Acme for pile accept, and even create scaled cards.
