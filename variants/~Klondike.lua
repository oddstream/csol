-- Klondike

dofile("variants/~Library.lua")

-- C sets variables 'BAIZE', 'STOCK', FAN_*, and tables to hold pile functions

POWER_MOVES = false
-- SEED = 3 -- 2 winnable draw three
-- SEED = 39880
-- SEED=35495

function BuildPiles()

    STOCK = AddPile("Stock", 1, 1, FAN_NONE, 1, 4)
    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)
    
    local pile

    FOUNDATIONS = {}
    for x = 4, 7 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileAccept(pile, 1)
        table.insert(FOUNDATIONS, pile)
    end

    TABLEAUX = {}
    local deal = 1
    for x = 1, 7 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        PileAccept(pile, 13)
        table.insert(TABLEAUX, pile)

        for n = 1, deal do
          local c = MoveCard(STOCK, pile)
          CardProne(c, true)
        end
        CardProne(Last(pile), false)
        deal = deal + 1
    end

end

function StartGame()
    STOCK_RECYCLES = 32767
end

-- CanTailBeMoved constraints (Tableau only)

function Tableau.CanTailBeMoved(tail)
    local c1 = Get(tail, 1)
    for i = 2, Len(tail) do
        local c2 = Get(tail, i)
        local err = DownAltColor(c1, c2)  if err then return false, err end
        c1 = c2
    end
    return true
end

-- CanTailBeAppended constraints

function Waste.CanTailBeAppended(pile, tail)
    if Len(tail) > 1 then
        return false, "The Waste can only accept a single card"
    end
    if CardOwner(Get(tail, 1)) ~= STOCK then
        return false, "The Waste can only accept cards from the Stock"
    end
    return true
end

function Foundation.CanTailBeAppended(pile, tail)
    if Len(pile) == 0 then
        local c1 = Get(tail, 1)
        if CardOrdinal(c1) ~= 1 then
            return false, "Foundation can only accept an Ace, not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = Get(tail, 1)
        local err = UpSuit(c1, c2)  if err then return false, err end
    end
    return true
end

function Tableau.CanTailBeAppended(pile, tail)
    if Len(pile) == 0 then
        local c1 = Get(tail, 1)
        if CardOrdinal(c1) ~= 13 then
            return false, "Empty Tableaux can only accept a King, not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = Get(tail, 1)
        local err = DownAltColor(c1, c2)  if err then return false, err end
    end
    return true
end

-- IsPileConformant

function Tableau.IsPileConformant(pile)
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = DownAltColor(c1, c2)  if err then return false, err end
        c1 = c2
    end
    return true
end

-- SortedAndUnSorted (_Tableau only)

function Tableau.SortedAndUnsorted(pile)
    local sorted = 0
    local unsorted = 0
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = DownAltColor(c1, c2)
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
