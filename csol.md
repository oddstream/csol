# csol

## Conventions

No typedefs.

Functions names are in PascalCase, ObjectVerb.

Hashed string heap for quick string comparisons?

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

Try a command-driven config files, one per variant, stored in a directory with filename = variant name, *eg*

Programming in Lua 4th Edition uses 5.3

Lastest version is 5.4.3

Maybe edit `luaconf.h` to use int and float the same size as raylib (which certainly uses float a lot).

```lua
-- Freecell
Description("Popular game, unusual because almost all deals are winnable")

local stock, p

stock = CreatePile("Stock", -2, -2, "None", {0,0,0})

p = CreatePile("Foundation", 4, 0, "None", {21,0,0})
p.SetAccept(p, "AC")

p = CreatePile("Tableau", 0, 1, "Down", {42,42,1})
p.Deal("UUUUUUU")
```
