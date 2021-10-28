-- Limited

dofile("variants/~Library.lua")

POWER_MOVES = true
SEED = 4  -- winnable
STOCK_DEAL_CARDS = 1
STOCK_RECYCLES = 32767

-- C sets variables 'BAIZE', 'STOCK', FAN_*

function BuildPiles()

    STOCK = AddPile("Stock", 1, 1, FAN_NONE, 2, 4)

    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)

    local pile

    FOUNDATIONS = {}
    for x = 5, 12 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileAccept(pile, 1)
        table.insert(FOUNDATIONS, pile)
    end

    TABLEAUX = {}
    for x = 1, 12 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for n = 1, 3 do
            MoveCard(STOCK, pile)
        end
    end

end

-- CanTailBeMoved constraints (Tableau only)

function Tableau.CanTailBeMoved(tail)
    if POWER_MOVES then
        local c1 = Get(tail, 1)
        for i = 2, Len(tail) do
            local c2 = Get(tail, i)
            local err = DownSuit(c1, c2) if err then return false, err end
            c1 = c2
        end
    else
        if Len(tail) > 1 then
            return false, "Can only move a single card"
        end
    end
    return true
end

-- CanTailBeAppended constraints

function Waste.CanTailBeAppended(pile, tail)
    if CardOwner(First(tail)) ~= STOCK then
        return false, "The Waste can only accept cards from the Stock"
    end
    return true
end

function Foundation.CanTailBeAppended(pile, tail)
    if Empty(pile) then
        local c1 = First(tail)
        if CardOrdinal(c1) ~= 1 then
            return false, "Foundation can only accept an Ace, not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = UpSuit(c1, c2) if err then return false, err end
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
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, n)
        local err = DownSuit(c1, c2) if err then return false, err end
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
        if err then
            unsorted = unsorted + 1
        else
            sorted = sorted + 1
        end
        c1 = c2
    end
    return sorted, unsorted
end

-- Actions

function Stock.Tapped(tail)
    if tail then
        MoveCard(STOCK, WASTE)
    end
end
