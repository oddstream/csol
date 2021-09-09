-- Freecell

FanDown = 3

Packs = 1

-- C sets variable 'baize'

function Build()
    local pile

    if type(AddPile) ~= "function" then
        io.stderr:write("Build cannot find function AddPile\n")
        return
    end

    io.stderr:write("in build\n")

    pile = AddPile("Cell", 200, 100, 0)
    pile = AddPile("Cell", 300, 100, 0)
    pile = AddPile("Cell", 400, 100, 0)
    pile = AddPile("Cell", 500, 100, 0)

    pile = AddPile("Foundation", 600, 100, 0)
    pile = AddPile("Foundation", 700, 100, 0)
    pile = AddPile("Foundation", 800, 100, 0)
    pile = AddPile("Foundation", 900, 100, 0)

    s = FindPile("Stock", 1)
    if s == nil then
        io.stderr:write("Build cannot find pile\n")
        return
    else
        io.stderr:write("Build found a pile\n")
    end

    pile = AddPile("Tableau", 200, 300, FanDown)
    DealUp(pile, 7)

    pile = AddPile("Tableau", 300, 300, FanDown)
    DealUp(pile, 7)

    pile = AddPile("Tableau", 400, 300, FanDown)
    DealUp(pile, 7)

    pile = AddPile("Tableau", 500, 300, FanDown)
    DealUp(pile, 7)

    pile = AddPile("Tableau", 600, 300, FanDown)
    DealUp(pile, 6)

    pile = AddPile("Tableau", 700, 300, FanDown)
    DealUp(pile, 6)

    pile = AddPile("Tableau", 800, 300, FanDown)
    DealUp(pile, 6)

    pile = AddPile("Tableau", 900, 300, FanDown)
    DealUp(pile, 2)

end
