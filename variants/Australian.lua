-- Australian

dofile("variants/~Library.lua")

function Wikipedia()
    return "https://en.wikipedia.org/wiki/Australian_Patience"
end

function BuildPiles()

    AddPile("Stock", 1, 1, FAN_NONE, 1, 4)
    AddPile("Waste", 2, 1, FAN_RIGHT3)
    
    for x = 4, 7 do
        local pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileLabel(pile, U[1])
    end

    for x = 1, 7 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN, MOVE_ANY)
        PileLabel(pile, U[13])
    end

end

function StartGame()

    StockRecycles(0)

    for _, pile in ipairs(Tableau.Piles) do
        for n = 1, 4 do
            local c = MoveCard(Stock.Pile, pile)
          end
      end

end

-- TailMoveError constraints (Tableau only)

function Tableau.TailMoveError(tail)
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
        local c2 = Get(tail, 1)
        local err = UpSuit(c1, c2) if err then return err end
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    if Empty(pile) then
        local c1 = First(tail)
        if CardOrdinal(c1) ~= 13 then
            return "Empty Tableaux can only accept a King, not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = Get(tail, 1)
        local err = DownSuit(c1, c2) if err then return err end
    end
    return nil
end

function Tableau.UnsortedPairs(pile)
    local unsorted = 0
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = DownSuit(c1, c2)
        if err then
            unsorted = unsorted + 1
        end
        c1 = c2
    end
    return unsorted
end

-- Actions

function Stock.TailTapped(tail)
    MoveCard(Stock.Pile, Waste.Pile)
end

