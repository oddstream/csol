-- Quick Win

--[[
    Style
    
    Lua globals are in UPPER_CASE, and use underscores between words
    function names are in MixedCase
    functions that apply to a type of pile are suffixed by _piletype
]]

dofile("variants/~Library.lua")

POWER_MOVES = false
STOCK_DEAL_CARDS = 1
STOCK_RECYCLES = 3

-- C sets variables 'BAIZE', 'STOCK', FAN_*, and tables to hold pile functions

function BuildPiles()

    if type(AddPile) ~= "function" then
        io.stderr:write("Build cannot find function AddPile\n")
        return
    end

    -- a stock pile is always created first, and filled with PACKS of shuffled cards
    STOCK = AddPile("Stock", 1, 1, FAN_NONE, 1, 4, {12,13})
    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)

    local pile

    FOUNDATIONS = {}
    for x = 4, 9, 1.5 do    -- slots don't have to be integers
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileAccept(pile, 1)
        table.insert(FOUNDATIONS, pile)
    end

    MoveCard(STOCK, FOUNDATIONS[1], 1, 0)

    TABLEAUX = {}
    for x = 1, 10 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for n = 1, 2 do
          MoveCard(STOCK, pile)
        end
        PileDemoteCards(pile, 13)
        PilePromoteCards(pile, 1)
    end
    for x = 1, 10 do
        pile = AddPile("Tableau", x, 4, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for n = 1, 2 do
          MoveCard(STOCK, pile)
        end
        PileDemoteCards(pile, 13)
        PilePromoteCards(pile, 1)
    end
end

function StartGame()
    io.stderr:write("StartGame\n")
    STOCK_RECYCLES = 3
end

-- CanTailBeMoved constraints (Tableau only)

function Tableau.CanTailBeMoved(tail)
    io.stderr:write("Tableau.CanTailBeMoved\n")

    local err
    local c1 = Get(tail, 1)
    for i = 2, Len(tail) do
        local c2 = Get(tail, i)
        err = DownSuit(c1, c2) if err then return false, err end
        c1 = c2
    end
    return true
end

--[[
function CanTailBeMoved(tail)
    local c1 = TailGet(tail, 1)
    local pile = CardOwner(c1)
    local fn = PileType(pile) .. ".CanTailBeMoved"
    -- io.stderr:write("type(" .. fn .. ") == " .. type(_G[fn]) .. "\n")
    if type(_G[fn]) == "function" then
        return _G[fn](pile, tail)
    else
        io.stderr:write(fn .. " is not a function\n")
    end
    return true
end
]]

-- CanTailBeAppended constraints

function Waste.CanTailBeAppended(pile, tail)
    if CardOwner(First(tail)) ~= STOCK then
        return false, "The Waste can only accept cards from the Stock"
    end
    return true
end

function Foundation.CanTailBeAppended(pile, tail)
    local err
    if Len(pile) == 0 then
        local c1 = First(tail)
        if CardOrdinal(c1) ~= 1 then
            return false, "Foundation can only accept an Ace, not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail, 1)
        err = UpSuit(c1, c2) if err then return false, err end
    end
    return true
end

function Tableau.CanTailBeAppended(pile, tail)
    if Empty(pile) then
        -- do nothing, empty accept any card
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = DownSuit(c1, c2) if err then return false, err end
    end
    return true
end

-- IsPileConformant (Tableau only)

function Tableau.IsPileConformant(pile)
    local err
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        err = DownSuit(c1, c2) if err then return false, err end
        c1 = c2
    end
    return true
end

-- SortedAndUnSorted (Tableau only)

function Tableau.SortedAndUnsorted(pile)
    local sorted = 0
    local unsorted = 0
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = DownSuit(c1, c2)
        if not err then
            sorted = sorted + 1
        else
            unsorted = unsorted + 1
        end
        c1 = c2
    end
    return sorted, unsorted
end

-- Actions

function Stock.Tapped(tail)
    if tail == nil then
        if STOCK_RECYCLES == 0 then
            return "No more Stock recycles"
        elseif 1 == STOCK_RECYCLES then
            Toast("Last Stock recycle")
        elseif 2 == STOCK_RECYCLES then
            Toast("One Stock recycle remaining")
        end
        if Len(WASTE) > 0 then
            while Len(WASTE) > 0 do
                MoveCard(WASTE, STOCK)
            end
            STOCK_RECYCLES = STOCK_RECYCLES - 1
          end
    else
        for i = 1, STOCK_DEAL_CARDS do
            MoveCard(STOCK, WASTE)
        end
    end
end

function AfterMove()
  -- io.stdout:write("AfterMove\n")
    for _, pile in ipairs(TABLEAUX) do
        if Len(pile) == 0 then
            if Len(WASTE) > 0 then
                MoveCard(WASTE, pile)
            elseif Len(STOCK) > 0 then
                MoveCard(STOCK, pile)
            end
        end
    end
    if Len(WASTE) == 0 then
        MoveCard(STOCK, WASTE)
    end
end
