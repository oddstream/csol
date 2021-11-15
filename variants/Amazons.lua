-- Amazons

--[[
    https://en.wikipedia.org/wiki/Amazons_(solitaire)
    Morehead and Mott-Smith, p179
]]

dofile("variants/~Library.lua")

POWER_MOVES = false
NUMBER_OF_COLORS = 4

function BuildPiles()

    AddPile("Stock", 6, 1, FAN_NONE, 1, 4, {2,3,4,5,6,13})

    local pile

    for x = 1, 4 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileLabel(pile, U[1])
    end

    for x = 1, 4 do
        pile = AddPile("Reserve", x, 2, FAN_DOWN)
        MoveCard(Stock.Pile, pile)
    end

end

function CalcPileIndex(piles, pile)
    for i, p in ipairs(piles) do
        if p == pile then
            return i
        end
    end
    io.stderr:write("CalcPileIndex cannot find pile\n")
end

-- TailMoveError constraints (Tableau only)

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
            return "An empty Foundation can only accept an Ace, not a " .. V[CardOrdinal(c1)]
        end
        local itarget = CalcPileIndex(Foundation.Piles, pile)
        local isource = CalcPileIndex(Reserve.Piles, CardOwner(c1))
        if isource ~= itarget then
            return "Aces can only be placed on the Foundation above"
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        -- work out the index of the target pile
        if CardOrdinal(c2) ~= 12 then
            local itarget = CalcPileIndex(Foundation.Piles, CardOwner(c1))
            local isource = CalcPileIndex(Reserve.Piles, CardOwner(c2))
            if isource ~= itarget then
                return "Cards can only be placed on the Foundation above"
            end
        end
        if CardSuit(c1) ~= CardSuit(c2) then
            return "Foundations build in suit"
        end
        if CardOrdinal(c1) == 1 and CardOrdinal(c2) == 7 then
            return nil
        elseif CardOrdinal(c1) + 1 == CardOrdinal(c2) then
            return nil
        else
            return "Foundations build up"
        end
    end
    return nil
end

-- Actions

function Stock.PileTapped(pile)
    for _, res in ipairs(Reserve.Piles) do
        MoveAllCards(res, Stock.Pile)
    end
end

function Stock.TailTapped(tail)
    for _, res in ipairs(Reserve.Piles) do
        MoveCard(Stock.Pile, res)
    end
end
