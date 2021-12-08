# gomps4

Towards a polymorphic solitaire engine in C99, [raylib](https://raylib.com/) with each variant configured by a small [Lua](https://lua.org) script. So-called because it's the fourth iteration of Gilbert Oddstream's Minimal Polymorphic Solitaire engine.

Development is currently stalled because of the difficulty of making decent scalable cards. gomps5 (written in Go + ebiten) is addressing this.

## Architecture

The fundamental objects are:

### Card

gomps4 creates a library of the required number of cards (which depends on the number of packs used, and if any cards were filtered out), thereafter any reference to a card really means 'a pointer into the card library'.

Cards are drawn either using a spritesheet or by Unicode card glyphs. Ideal would be drawing SVG cards direct to the screen (or, rather, creating resolution-dependant spritesheets at runtime), but I can't find a suitable SVG library.

### Pile

Each pile is a stack of card pointers. Piles come in several types: Cell, Discard, Foundation, Reserve, Stock, Tableau and Waste. Much of the functionality for each type is hard-wired, for example, Cells can only contain a single card, you cannot take a card from a Foundation, you cannot move a card to a Reserve or Stock. Things get a little more interesting for Tableau pile types, which are customised for each type of game using in-built or external scripts.

The different pile types are implemented using a the lightweight object-oriented trick of using embedded structs and virtual function pointer tables.

### Baize

Contains a list of piles

### Scripting each variant

Each different game is implemented in it's own script (some of which are built-in, coded in C, but most of which are in external Lua files).

### User Interface

## What the C does

* Handles all the graphics and sounds
* Moves the cards from pile to pile
* Turns the cards face up/down
* Defines categories of piles (Cell, Discard, Foundation, Reserve, Stock, Tableau) and enforces the rules that apply to them (for example, a Cell can only contain a single card of any type)
* Creates the cards, sorts them and places them into the Stock pile

## What the script does

### Procedures implemented in script that gomps4 invokes

```Lua
BuildPiles

StartGame

AfterMove
```

### Functions provided by gomps4 for use by script

```Lua
AddPile

PileAccept

PileType

MoveCard

MoveCards

SwapCards

CardPairs

CardColor

CardOrdinal

CardSuit

CardProne

CardOwner

Get

Len

First

Last

Empty

StockRecycles

Toast
```

### Global variables defined by gomps4 for use by script

```Lua
FAN_*

MOVE_*

Cell, Discard, Foundation, Reserve, Tableau, Stock, Waste tables
```

### Internal global variables

```Lua
BAIZE
```

## Command line flags

| Flag               | Example        | Description                                  |
| ------------------ | -------------- | -------------------------------------------- |
| --width, -w        | --width 640    | sets the window width                        |
| --height, --height | -h 1024        | sets the window height                       |
| --variant, -v      | -v "Busy Aces" | specifies the variant to load                |
| --pack             | pack=retro     | sets the pack of cards to use                |
| --noload           | --noload       | stops the loading of saved.txt               |
| --nosave           | --nosave       | stops the creation of saved.txt when exiting |
| --nolerp           | --nolerp       | stops the cards animating                    |
| --noflip           | --noflip       | stops the cards flipping                     |
|                    |                |                                              |

## C to Lua object model

### Option 1 : Object handles

The C creates, manages and frees objects that represent piles, cards and arrays of dragged cards. Handles to these objects are passed (as Lua 'light user data') to Lua functions. These functions, in turn, call C-provided functions to get properties of the objects. For example, C might call the Lua function `Tableau.IsPileConformant(pile)`, passing a handle to the pile to be checked. The Lua function iterates through the cards in the pile, like this:

```Lua
for i = 1, Len(pile) do
    local card = Get(pile, i)
    local ord = CardOrdinal(card)
end
```

where `Len` and `Get` and `CardOrdinal` are functions provided by C, that return an integer, a handle to a card object and an number in the range 1 .. 13, respectively.

This approach is fast and flexible, but does require that the script writer knows the constants and function calls available to the used.

### Option 2 : Tables for Pile, Tail and Card

Each Pile, Card or Tail object passes from C to Lua would be constructed (by C) as a Lua table, so that you could have expressions like:

```Lua
if firstcard.color ~= secondcard.color then
    return "cards must be in alternate colors"
end
```

A sequence of cards would be a Lua table array of cards, so you could have:

```Lua
for _,c in ipairs(tail) do
    someFunctionWithCard(card)
end
-- or
for i=1, #tail do
    someFunctionWithCard(tail[i])
end
```

At first sight, this seems good, but it falls apart when accessing a card's owner (each card is owned by a pile) because `card.owner` would need to be a pile, represented as a Lua table. The solution to this leads to option 3:

## Option 3 : Shadow object tree

The C maintains a complete copy of the baize, piles, and cards as a tree-shaped Lua table.

The Lua scripts are given access to both the root and branches of the table.

This gets ugly when two things happen (1) the Lua script moves a card; it would seem natural to move it in the table, but then gomps4 would have to parse the tree and update the cards positions, and (2) gomps4 would have the overhead of rebuilding the tree after everu user move (including undo).

Also, a tail of cards being dragged would fall outside this tree.

### Conclusion

Using object handles may seem a little clunky, but it offers the best balance of simplicity, effiency and flexibility.

## History

First there was an [online version](https://oddstream/games/Solitaire) version written in vanilla JavaScript, with scalable SVG graphics. There was an html file for each game variant, and these html files were merged from a standard header, a set of game rules and a standard footer. The variant rules were stored in json tables. Being Javascript, the code grew from something quite well structured, into a seething mess.

Second, there was an Android version written in [Lua](https://www.lua.org/) and using the [Solar2D](https://solar2d.com/) retained mode engine. It gets good reviews and usage in the Google Play Store.

Third, there was a version written in Go, using the immediate mode [Ebiten](https://ebiten.org) graphics/game engine. The intention was that this version would replace the previous two versions, and provide Linux, Windows, Android and browser-based versions from the same code base. If I had a Mac there would have been iOS and Mac versions, too. But I got discouraged by Go.

This (fourth) version is different because it's much simpler, is written in good old dependable C99, and uses Lua scripts to configure each variant, rather than relying on static tables embedded in the executable. In theory, a player can create and define their own variant, or tweak an existing one, just by editing the Lua scripts.

## Variants

It may eventually know how to play:

* Aces and Kings
* Acme
* Algerian
* Alhambra
* American Toad (also The Toad)
* Australian
* Baker's Dozen (also Baker's Dozen Relaxed)
* Bisley
* Bristol (also Dover)
* Canfield (also Acme, Storehouse)
* Cruel, Ripple Fan
* Duchess
* Gay Gordons
* Quick Win (an easy to win game, for debugging)
* Fortune's Favor
* Forty Thieves (also Busy Aces, Fortune's Favor, Forty and Eight, Josephine, Maria, Limited, Lucas, Red and Black)
* Freecell (also Eight Off, Freecell Easy)
* Klondike (also Draw One, Draw Three, Batsford, Double Klondike, Gargantua, Thumb and Pouch, Thoughtful)
* La Belle Lucie (and Trefoil, The Fan)
* Mistress and Mrs Mop
* Penguin
* Scorpion, Wasp
* Simple Simon
* Spider (also Beetle, Spiderette, Spider One Suit, Spider Two Suits, Will o' the Wisp)
* Yukon (also Yukon Relaxed)

Some variants have been tried and discarded as being a bit silly:

* Agnes Sorel
* Giant
* King Albert
* Raglan

(I don't see the point of games that you almost certainly can't win; I like ones that have a 33-66% chance of winnning.)

Some will never make it here because they are just poor games:

* Accordian
* Pyramid (or any card matching variant)

## Other features

* Unlimited undo, without penalty. Also, you can restart a deal without penalty.
* Bookmarking positions (really good for games like Freecell, Penguin or Simple Simon)
* Scalable (TODO), fixed-size and retro card designs
* ~~Movable card highlighting (the more useful a move looks, the more the card gets highlighted)~~ This has not been included because the author feels this ruins the essence of playing solitaire.
* Statistics (including percent complete and streaks - streaks are great) (TODO)
* Cards spin and flutter when you complete a game, so you feel rewarded and happy (TODO)
* Turn-offable one tap interface. Partial. In order to keep the essence of solitaire play, single tapping on a tableau/cell/waste card will try and send that card to a foundation, nothing more.
* Slightly randomized sounds (TODO)
* Automatic saving and reloading of the game in progress
* Pile scrunching, and a draggable baize; if cards spill out of view to the bottom or right of the screen, just drag the baize to move them into view

A lot a features have been tried and discarded, in order to keep the game (and player) focused. *Weniger aber besser*, as [Dieter Rams](https://en.wikipedia.org/wiki/Dieter_Rams) taught us. Design is all about saying "no", as Steve Jobs preached. Just because a feature *can* be implemented, does not mean it *should* be. Movable card highlighting is one notable feature that fell under this bus.

## FAQ

### What makes this different from the other solitaire implementations?

This solitaire is all about [Flow](https://en.wikipedia.org/wiki/Flow_(psychology)). Anything that distracts from your interaction with the flow of the game has been either been tried and removed or not included. There are no adverts, complicated hard-to-scan cards, distracting background, unnecessary information, or timers.

"A gimmick is a novel device or idea designed primarily to attract attention or increase appeal, often with little intrinsic value."

Also, I'm trying to make games authentic, by taking the rules from reputable sources and implementing them exactly.

### Why are the graphics so basic?

Anything that distracts from your interaction with the flow of the game, or the ability to scan a deck of cards, has either been tried and removed, or not included.

This includes: fancy card designs (front and back), changing the screen/baize background, keeping an arbitrary score, distracting graphics on the screen.

The user interface tries to stick to the Material Design guidelines, and so is minimal and tactile. I looked at a lot of the other solitaire websites out there, and think how distracting some of them are. Features seem to have been added because the developers thought they were cool; they never seem to have stopped to consider that just because they *could* implement a feature, that they *should*.

### Sometimes the cards are really huge or really tiny

Either resize your browser/desktop window (if using scalable cards) or change the settings to fixed size cards. (TODO)

### The rules for a variation are wrong

There's no ISO or ANSI or FIDE-like governing body for solitaire; so there's no standard set of rules. Other implementations vary in how they interpret each variant. For example, some variants of American Toad build the tableau down by suit, some by alternate color. So, rather than just making this stuff up, I've tried to find a well researched set of rules for each variant and stick to them, leaning heavily on Jan Wolter (RIP, and thanks for all the fish), David Parlett and Thomas Warfield. Where possible, I've implemented the games from the book "The Complete Book of Solitaire and Patience Games" by Albert Morehead and Geoffrey Mott-Smith.

### Keyboard shortcuts?

* `Backspace` - undo
* `N` - new deal (resigns current game, if started)
* `R` - restart deal
* `F` - find a new game
* `S` - save current position ('bookmark')
* `L` - load/return to a previously saved position
* `C` - collect cards to the foundations
* `0` - retro cards
* `1` - small sized cards
* `2` - medium sized cards
* `3` - large size cards
* `4` - four-color cards drawn with Unicode glyphs (clubs: black, diamonds: firebrick red, hearts: darkorchid, spades: darkgreen)
* `5` - two-color Unicode cards
* `Esc` - exit (current game and position are saved)

### What about scores?

Nope, the software doesn't keep an arbitary score. Too confusing. Just the number of moves made, number of wins, and your winning streak (streaks are great). A game isn't counted until you move a card. Thereafter, if you ask for a new deal, that counts as a loss.

You *can* cheat the score system by restarting a deal and then asking for a new deal, but that's on you.

### But you can cheat

You can when playing with actual cards, too. Cheat if you like; I'm not your mother.

### What about a timer?

Nope, there isn't one of those. Too stressful. Solitaire is also called *patience*; it's hard to feel patient when you're pressured by a clock.

### Is the game rigged?

No. The cards are shuffled randomly using a Fisher-Yates shuffle driven by a Park-Miller pseudo random number generator,
which is in itself seeded by a random number. This mechanism was tested and analysed to make sure it produced an even distribution of shuffled cards.

There are 80658175170943878571660636856403766975289505440883277824000000000000 possible deals of a pack of 52 playing cards; you're never going to play the same game twice, nor indeed play the same game that anyone else ever has, or ever will.

### Any hints and tips?

* For games that start with face down cards (like Klondike or Yukon) the priority is to get the face down cards turned over.
* For games that start with a block of cards in the tableaux and only allow single cards to be moved (like Forty Thieves or Simple Simon), the priority is usually to open up some space (create empty tableaux piles) to allow you to juggle cards around.
* For Forty Thieves-style games, the *other* priority is to minimize the number of cards in the waste pile.
* For puzzle-type games (like Freecell, Simple Simon and Mistress Mop), take your time and think ahead.
* For games with reshuffles (like Cruel and Perseverance) you need to anticipate the effects of the reshuffle.
* Use undo and bookmark. Undo isn't cheating; it's improvising, adapting and overcoming.

## Acknowledgements

Original games by Jan Wolter, David Parlett, Paul Alfille, Art Cabral, Albert Morehead, Geoffrey Mott-Smith, Zach Gage and Thomas Warfield.

Retro card back designs by [Leslie Kooy](https://www.lesliekooy.com/), jazz cup pattern by Gina Ekiss.

Sounds and card faces by [kenney.nl](https://www.kenney.nl/assets) and Victor Vashenko
