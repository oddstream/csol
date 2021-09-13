-- Freecell

FanNone = 0
FanDown = 1

Packs = 1

-- C sets variable 'BAIZE'

function Build()
    local pile

    if type(AddPile) ~= "function" then
        io.stderr:write("Build cannot find function AddPile\n")
        return
    end

    s = FindPile("Stock", 1)
    if s == nil then
        io.stderr:write("Build cannot find Stock pile\n")
        return
    else
        io.stderr:write("Build found a pile\n")
        MovePileTo(s, 10, 100)
    end

    for x = 100, 400, 100 do
        pile = AddPile("Cell", x, 100, FanNone)
    end

    for x = 500, 800, 100 do
        pile = AddPile("Foundation", x, 100, FanNone)
    end

    for x = 100, 400, 100 do
        pile = AddPile("Tableau", x, 300, FanDown)
        DealUp(pile, 7)
    end

    for x = 500, 800, 100 do
        pile = AddPile("Tableau", x, 300, FanDown)
        DealUp(pile, 5)
    end

end
