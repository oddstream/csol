-- Limited

dofile("variants/~Library.lua")

POWER_MOVES = true
NUMBER_OF_COLORS = 4
-- SEED = 4  -- winnable
STOCK_DEAL_CARDS = 1
STOCK_RECYCLES = 32767

-- C sets variables 'BAIZE', 'STOCK', FAN_*

function BuildPiles()

    AddPile("Stock", 1, 1, FAN_NONE, 2, 4)

    AddPile("Waste", 2, 1, FAN_RIGHT3)

    local pile

    for x = 5, 12 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileLabel(pile, U[1])
    end

    for x = 1, 12 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        for n = 1, 3 do
            MoveCard(Stock.Pile, pile)
        end
    end

end

-- TailMoveError constraints (Tableau only)

function Tableau.TailMoveError(tail)
    if POWER_MOVES then
        for _, c in ipairs(CardPairs(tail)) do
            local err = DownSuit(c[1], c[2]) if err then return err end
        end
    else
        if Len(tail) > 1 then
            return "Can only move a single card"
        end
    end
    return nil
end

-- TailAppendError constraints

function Waste.TailAppendError(pile, tail)
    if CardOwner(First(tail)) ~= Stock.Pile then
        return "The Waste can only accept cards from the Stock"
    end
    return nil
end

function Foundation.TailAppendError(pile, tail)
    if Empty(pile) then
        local c1 = First(tail)
        if CardOrdinal(c1) ~= 1 then
            return "Foundation can only accept an Ace, not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = UpSuit(c1, c2) if err then return err end
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    if Empty(pile) then
        -- do nothing, empty accept any card
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = DownSuit(c1, c2) if err then return err end
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

function Stock.TailTapped(tail)
    MoveCard(Stock.Pile, Waste.Pile)
end
