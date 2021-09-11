-- Freecell

FanNone = 0
FanDown = 1

Packs = 1

-- C sets variable 'baize'

function Build()
    local pile

    if type(AddPile) ~= "function" then
        io.stderr:write("Build cannot find function AddPile\n")
        return
    end

    s = FindPile("Stock", 1)
    if s == nil then
        io.stderr:write("Build cannot find pile\n")
        return
    else
        io.stderr:write("Build found a pile\n")
    end

    for x = 200, 500, 100 do
        pile = AddPile("Cell", x, 100, FanNone)
    end

    for x = 600, 900, 100 do
        pile = AddPile("Foundation", x, 100, FanNone)
    end

    for x = 200, 500, 100 do
        pile = AddPile("Tableau", x, 300, FanDown)
        DealUp(pile, 7)
    end

    for x = 600, 900, 100 do
        pile = AddPile("Tableau", x, 300, FanDown)
        DealUp(pile, 5)
    end

end
