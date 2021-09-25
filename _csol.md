# Csol

## Undo

saved baize is an array (of Pile*) of array (of Card* in each pile)

don't store baize state, it can be calculated

don't store a crc

don't store - for the moment - stock.recycles or pile.accept

so no need for a saved baize header, or saved pile header

serialize later

## UI

copy gosol UI (it was hard-won, and it works)

Array of Containers

## Next

==323103==    by 0x1136A2: ArrayNew (array.c:10)
==323103==    by 0x113B7F: ArrayClone (array.c:133)
==323103==    by 0x11A3B2: SnapshotNew (undo.c:20)
==323103==    by 0x11A4EF: BaizeUndoPush (undo.c:49)
==323103==    by 0x11477B: BaizeResetState (baize.c:214)
==323103==    by 0x116D13: main (main.c:198)

Do more in Lua. AutoMoves(AcceptFirstPush, AutoFillFrom), Spider, Accept/Recycles?

CanAcceptTail
        if the tail is conformant, then only need to check card pair (pile peek, c0) conformant

Remove CardTapped from vtable, all actions are in Lua:
        Send card(s) to waste
        Send cards to tableaux
        Recall and redeal tableaux
        Send card to foundation

Keep PileTapped because that Stock recycle (it's only use) is baked in
CanAcceptTail is still in vtable because some pile behaviour is baked in:
        Waste can only accept cards from stock,
        Cells can only have one card,
        Foundations can only have 13 cards,
        Stock can never accept a card,

- [ ] check stack height before and after
- [ ] C - BaizeCollect()
- [ ] PowerMoves
- [ ] dragging 4 and 5 separately after they'd been DragCancelled, other dragging confusion
- [ ] _Generic
- [ ] don't like passing Baize->L around, now have ->owner
- [ ] draw recycle symbol
- [ ] CardSetPos() problem
- [ ] ttf Font problem - try a raylib font
- [ ] title bar (hamburger : variant : collect undo)
- [ ] status bar (stock waste : moves : complete)
- [x] toast
- [x] Spider
        [x] empty piles check
        [x] Lua to okay if tail can be dragged
        [x] CardTapped on stock
        [x] suit filter 1-4
        [x] foundation accept
- [x] refuse to start dragging a transitioning card
- [x] MoveCard(STOCK, pile, prone)
- [x] CardTapped(pileCategory, tail)
        Lua function called by C
        returns true/false if cards moved, error string
        MoveCards(SOURCE, DEST, cards) function callable from Lua
- [x] N - BaizeNewDeal()
- [x] R - BaizeRestartDeal()
- [x] S - BaizeSavePosition()
- [x] L - BaizeLoadPosition()
- [x] undo stack of `Array` of `Array` of `Card*`
- [x] window size, window color, card scale, card set, last variant in Lua settings file (simple, writable) LoadSettings(), SaveSettings()
- [x] card scaling
- [x] CheckDrag, CanAcceptTail should return an error string for toasting later
- [x] state (no, can be calculated)
- [x] more cardsMoved++ (eg PileMoveCards)
- [x] error return from some functions could be a toastable string; return bool then get last error
- [x] baize dragging
- [x] drag card from Stock to Waste, so chk functions need to know where a card is coming from?
- [x] need drag none/drag single/drag single or pile/drag many flags, set from Lua
- [x] baize and card scale, uses slots in .lua files
- [x] retire info width & height
- [x] kenney.nl playing card spritesheet
- [x] starting to drag a card that's transitioning, ends up displaced rather than finding it's way back to the pile
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

## Problems

- [ ] why does LoadFont cause a segmentation fault?
- [ ] no GetWindowWidth, Height, making resize useless

## Backup

```bash
gilbert@iMac:/media/gilbert/GREENTHUMB$ git clone /home/gilbert/projects/csol csol
```

`git bundle`

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