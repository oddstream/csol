# csol

## Next

- Freecell.lua
- CreatePile function in C callable from Lua
- minimal Makefile

## Conventions

No typedefs on structs or enums.

Functions names are in PascalCase, ObjectVerb.

Hashed string heap for quick string comparisons?

Use "AC", "2H", "XD", "KS" shorthand for cards.

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
