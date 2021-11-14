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

SEED = 42

-- C sets variables 'BAIZE', FAN_*, and tables to hold pile functions

local function DemoteCards(pile, ord)
    local hasChanged
    local itemCount = Len(pile)
    repeat
        hasChanged = false
        itemCount = itemCount - 1
        for i = 1, itemCount do
            local c1 = Get(pile, i)
            local c2 = Get(pile, i + 1)
            if CardOrdinal(c1) ~= ord and CardOrdinal(c2) == ord then
                SwapCards(c1, c2)
                hasChanged = true
            end
        end
    until not hasChanged 
end

local function PromoteCards(pile, ord)
    local hasChanged
    local itemCount = Len(pile)
    repeat
        hasChanged = false
        itemCount = itemCount - 1
        for i = 1, itemCount do
            local c1 = Get(pile, i)
            local c2 = Get(pile, i + 1)
            if CardOrdinal(c1) == ord and CardOrdinal(c2) ~= ord then
                SwapCards(c1, c2)
                hasChanged = true
            end
        end
    until not hasChanged 
end

local function SortCards(pile)
    local hasChanged
    local itemCount = Len(pile)
    repeat
        hasChanged = false
        itemCount = itemCount - 1
        for i = 1, itemCount do
            local c1 = Get(pile, i)
            local c2 = Get(pile, i + 1)
            if CardOrdinal(c1) < CardOrdinal(c2) then
                SwapCards(c1, c2)
                hasChanged = true
            end
        end
    until not hasChanged 
end

function BuildPiles()

    io.stderr:write("BuildPiles\n")

    if type(AddPile) ~= "function" then
        io.stderr:write("Build cannot find function AddPile\n")
        return
    end

    -- a stock pile is always created first, and filled with PACKS of shuffled cards
    AddPile("Stock", 1, 1, FAN_NONE, 2, 4, {12,13})
    if Stock.Pile then
        io.stderr:write("Stock.Pile autocreated\n")
    else
        io.stderr:write("Stock.Pile not autocreated\n")
    end
    if Stock.Piles[1] then
        io.stderr:write("Stock.Piles[1] autocreated\n")
    else
        io.stderr:write("Stock.Piles[1] not autocreated\n")
    end

    AddPile("Waste", 2, 1, FAN_RIGHT3)
    if Waste.Pile then
        io.stderr:write("Waste.Pile autocreated\n")
    else
        io.stderr:write("Waste.Pile not autocreated\n")
    end
    if Waste.Piles[1] then
        io.stderr:write("Waste.Piles[1] autocreated\n")
    else
        io.stderr:write("Waste.Piles[1] not autocreated\n")
    end

    for x = 4, 11 do    -- slots don't have to be integers
        local pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileLabel(pile, U[1])
    end

    for x = 4, 11 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN)
    end
    for x = 4, 11 do
        local pile = AddPile("Tableau", x, 4, FAN_DOWN)
    end

    PromoteCards(Stock.Pile, 1)
end

function StartGame()
    io.stderr:write("StartGame\n")
    STOCK_RECYCLES = 3

    -- MoveCard(Stock.Pile, Foundation.Piles[1], 1, 0)

    for _, pile in ipairs(Tableau.Piles) do 
        for n = 1, 5 do
            MoveCard(Stock.Pile, pile)
        end
        -- DemoteCards(pile, 11)
        -- PromoteCards(pile, 1)
        SortCards(pile)
        Refan(pile)
    end
--[[
    for _, pile in ipairs(Tableau.Piles) do 
        for n = 1, 5 do
            CardProne(Get(pile, n), true)
        end
        CardProne(Last(pile), false)
    end
]]

--[[
    local cp = CardPairs(Tableau.Piles[1])
    io.stderr:write("#CardPairs ", #cp .. "\n");
    for _, v in pairs(cp) do
        assert(v[1])
        assert(v[2])
        -- io.stderr:write("#Pair ", #v .. "\n");
        -- io.stderr:write("1 " .. CardOrdinal(v[1]) .. "\n");
        -- io.stderr:write("2 " .. CardOrdinal(v[2]) .. "\n");
        io.stderr:write("{" .. CardOrdinal(v[1]) .. "," .. CardOrdinal(v[2]) .. "}\n")
    end

    cp = CardPairs(Waste.Pile)
    io.stderr:write("#CardPairs ", #cp .. "\n");
    for _, v in pairs(cp) do
        io.stderr:write("{" .. CardOrdinal(v[1]) .. "," .. CardOrdinal(v[2]) .. "}\n")
    end
]]
end

-- TailMoveError

function Tableau.TailMoveError(tail)
    io.stderr:write("Tableau.TailMoveError\n")
--[[
    local err
    local c1 = Get(tail, 1)
    for i = 2, Len(tail) do
        local c2 = Get(tail, i)
        err = DownSuit(c1, c2) if err then return err end
        c1 = c2
    end
]]
    for _, c in ipairs(CardPairs(tail)) do
        local err = DownSuit(c[1], c[2]) if err then return err end
    end
    return nil
end

--[[
function TailMoveError(tail)
    local c1 = TailGet(tail, 1)
    local pile = CardOwner(c1)
    local fn = PileType(pile) .. ".TailMoveError"
    -- io.stderr:write("type(" .. fn .. ") == " .. type(_G[fn]) .. "\n")
    if type(_G[fn]) == "function" then
        return _G[fn](pile, tail)
    else
        io.stderr:write(fn .. " is not a function\n")
    end
    return nil
end
]]

-- TailAppendError constraints

function Waste.TailAppendError(pile, tail)
    io.stderr:write("Waste.TailAppendError\n")
    if CardOwner(First(tail)) ~= Stock.Pile then
        return "The Waste can only accept cards from the Stock"
    end
    return nil
end

function Foundation.TailAppendError(pile, tail)
    io.stderr:write("Foundation.TailAppendError\n")
    if Empty(pile) then
        local c1 = First(tail)
        if CardOrdinal(c1) ~= 1 then
            return "Foundation can only accept an Ace, not a " .. V[CardOrdinal(c1)]
        end
    else
        local err = UpSuit(Last(pile), First(tail)) if err then return err end
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    io.stderr:write("Tableau.TailAppendError\n")
    if Empty(pile) then
        -- do nothing, empty accept any card
    else
        local err = DownSuit(Last(pile), First(tail)) if err then return err end
    end
    return nil
end

-- PileConformantError (Tableau only)

function Tableau.PileConformantError(pile)
    for _, c in ipairs(CardPairs(pile)) do
        local err = DownSuit(c[1], c[2]) if err then return err end
    end
    return nil
end

-- SortedAndUnSorted (Tableau only)

function Tableau.SortedAndUnsorted(pile)
    local sorted = 0
    local unsorted = 0
    for _, c in ipairs(CardPairs(pile)) do
        local err = DownSuit(c[1], c[2])
        if err then
            unsorted = unsorted + 1
        else
            sorted = sorted + 1
        end
    end
    return sorted, unsorted
end

-- Actions

function Stock.PileTapped(pile)
    io.stderr:write("Stock.PileTapped\n")

    if STOCK_RECYCLES == 0 then
        return "No more Stock recycles"
    elseif 1 == STOCK_RECYCLES then
        Toast("Last Stock recycle")
    elseif 2 == STOCK_RECYCLES then
        Toast("One Stock recycle remaining")
    end
    if Len(Waste.Pile) > 0 then
        while Len(Waste.Pile) > 0 do
            MoveCard(Waste.Pile, Stock.Pile)
        end
        STOCK_RECYCLES = STOCK_RECYCLES - 1
    end
end

function Stock.TailTapped(tail)
    io.stderr:write("Stock.TailTapped\n")
    for i = 1, STOCK_DEAL_CARDS do
        MoveCard(Stock.Pile, Waste.Pile)
    end
end

--[[
function Waste.TailTapped(tail)
    io.stderr:write("Waste.TailTapped\n")
    if Len(tail) == 1 then
        for _, pile in ipairs(Foundation.Piles) do
            if not Foundation.TailAppendError(pile, tail) then
                MoveCard(CardOwner(First(tail)), pile)
                break
            end
        end
    end
end

function Tableau.TailTapped(tail)
    io.stderr:write("Tableau.TailTapped\n")
    if Len(tail) == 1 then
        for _, pile in ipairs(Foundation.Piles) do
            if not Foundation.TailAppendError(pile, tail) then
                MoveCard(CardOwner(First(tail)), pile)
                break
            end
        end
    end
end
]]

function AfterMove()
  io.stdout:write("AfterMove\n")
    for _, pile in ipairs(Tableau.Piles) do
        if Empty(pile) then
            if Len(Waste.Pile) > 0 then
                MoveCard(Waste.Pile, pile)
            elseif Len(Stock.Pile) > 0 then
                MoveCard(Stock.Pile, pile)
            end
        end
    end
    if Empty(Waste.Pile) then
        MoveCard(Stock.Pile, Waste.Pile)
    end
end
