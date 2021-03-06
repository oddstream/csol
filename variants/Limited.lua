-- Limited

dofile("variants/~Library.lua")

-- C sets variables 'BAIZE', 'STOCK', FAN_*

function Wikipedia()
    return "https://en.wikipedia.org/wiki/Forty_Thieves_(solitaire)"
end

function BuildPiles()

    AddPile("Stock", 1, 1, FAN_NONE, 2, 4)
    AddPile("Waste", 2, 1, FAN_RIGHT3)

    for x = 5, 12 do
        local pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileLabel(pile, U[1])
    end

    for x = 1, 12 do
        AddPile("Tableau", x, 2, FAN_DOWN, MOVE_ONE_PLUS)
    end

end

function StartGame()
    StockRecycles(0)
    for _, pile in ipairs(Tableau.Piles) do
        for n = 1, 3 do
            MoveCard(Stock.Pile, pile)
        end
    end
end

-- TailMoveError constraints (Tableau only)

function Tableau.TailMoveError(tail)
    if Len(tail) > 1 then
        for _, c in ipairs(CardPairs(tail)) do
            local err = DownSuit(c[1], c[2]) if err then return err end
        end
    end
    return nil
end

-- TailAppendError constraints

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

function Tableau.UnsortedPairs(pile)
    local unsorted = 0
    for _, c in ipairs(CardPairs(pile)) do
        local err = DownSuit(c[1], c[2])
        if err then
            unsorted = unsorted + 1
        end
    end
    return unsorted
end

-- Actions

function Stock.TailTapped(tail)
    MoveCard(Stock.Pile, Waste.Pile)
end
