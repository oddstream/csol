-- Sir Tommy, Old Patience, Try Again

--[[
  https://en.wikipedia.org/wiki/Sir_Tommy

  Cards are dealt one at a time.
  When an ace turns up, it forms a foundation which builds up to King regardless of suit.
  Four such foundations should be built.
  A card that cannot yet be placed on the foundation is placed onto one of four wastepiles;
  once placed, it cannot be moved, but the top card of each wastepile remains available
  to be placed on a foundation.

  The game is won if all cards are emptied from the wastepiles and built on the foundations.
]]

dofile("variants/~Library.lua")

function Wikipedia()
    return "https://en.wikipedia.org/wiki/Sir_Tommy"
end

function BuildPiles()

    AddPile("Stock", 1, 1, FAN_NONE, 1, 4)

    AddPile("Waste", 2, 1, FAN_RIGHT3)

    for x = 4, 7 do
        local pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileLabel(pile, U[1])
    end

    for x = 4, 7 do
        AddPile("Tableau", x, 2, FAN_DOWN, MOVE_ONE)
    end

end

function StartGame()
    StockRecycles(0)
end

-- TailMoveError constraints (Tableau only)

function Tableau.TailMoveError(tail)
    if Len(tail) > 1 then
        return "Can only move a single card"
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
        -- build up regardless of suit
        local c1 = Last(pile)
        local c2 = First(tail)
        if CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
            return "Foundations build up"
        end
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    if CardOwner(First(tail)) ~= Waste.Pile then
        return "Cards can only be moved here from the Waste"
    end
    return nil
end

-- Actions

function Stock.TailTapped(tail)
    -- only allow one card at a time in waste
    if Empty(Waste.Pile) then
        MoveCard(Stock.Pile, Waste.Pile)
    else
        Toast("The Waste must be emptied first")
    end
end
