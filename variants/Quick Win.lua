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
        for n = 1, 3 do
          MoveCard(STOCK, pile)
        end
        PileDemoteCards(pile, 13)
        PilePromoteCards(pile, 1)
    end
    for x = 1, 10 do
        pile = AddPile("Tableau", x, 4, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for n = 1, 1 do
          MoveCard(STOCK, pile)
        end
        PileDemoteCards(pile, 13)
        PilePromoteCards(pile, 1)
    end
end

function StartGame()
    io.stderr:write("StartGame\n")
    STOCK_RECYCLES = 3

    local cp = CardPairs(TABLEAUX[1])
    io.stderr:write("#CardPairs ", #cp .. "\n");
    for _, v in pairs(cp) do
        assert(v[1])
        assert(v[2])
        -- io.stderr:write("#Pair ", #v .. "\n");
        -- io.stderr:write("1 " .. CardOrdinal(v[1]) .. "\n");
        -- io.stderr:write("2 " .. CardOrdinal(v[2]) .. "\n");
        io.stderr:write("{" .. CardOrdinal(v[1]) .. "," .. CardOrdinal(v[2]) .. "}\n")
    end

    cp = CardPairs(WASTE)
    io.stderr:write("#CardPairs ", #cp .. "\n");
    for _, v in pairs(cp) do
        io.stderr:write("{" .. CardOrdinal(v[1]) .. "," .. CardOrdinal(v[2]) .. "}\n")
    end
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
    if CardOwner(First(tail)) ~= STOCK then
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

function Waste.Tapped(tail)
    if not tail or Len(tail) > 1 then
        return
    end
    for _, pile in ipairs(FOUNDATIONS) do
        if Foundation.TailAppendError(pile, tail) then
            MoveCard(CardOwner(First(tail)), pile)
            break
        end
    end
end

function Tableau.Tapped(tail)
    if not tail or Len(tail) > 1 then
        return
    end
    for _, pile in ipairs(FOUNDATIONS) do
        if Foundation.TailAppendError(pile, tail) then
            MoveCard(CardOwner(First(tail)), pile)
            break
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
