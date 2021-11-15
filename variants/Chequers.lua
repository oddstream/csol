-- Chequers

--[[
    https://politaire.com/chequers
    https://politaire.com/help/chequers
]]

dofile("variants/~Library.lua")

POWER_MOVES = false
NUMBER_OF_COLORS = 4
STOCK_RECYCLES = 0

if not table.contains then
    function table.contains(tab, val)
        for index, value in ipairs(tab) do
            if value == val then
                return true, index
            end
        end
        return false, 0
    end
end

-- C sets variables 'BAIZE', 'STOCK', FAN_*

function BuildPiles()

    AddPile("Stock", -5, -5, FAN_NONE, 2, 4)

    local pile
    local card

--[[
There are two groups of foundation piles.

Ace Foundation:

The ace foundation contains four piles.
Any ace may be moved to any empty pile in the ace foundation.
Each pile must start with a different suit.

A card may be added onto a ace foundation pile if it is one higher than the old top card of the pile and of the same suit.
Thus, the only card that could be played on a 6♦ would be a 7♦.
No pile may contain more than 13 cards.

King Foundation:

The king foundation contains four piles.
Any king may be moved to any empty pile in the king foundation.
Each pile must start with a different suit.

A card may be added onto a king foundation pile if it is one lower than the old top card of the pile and of the same suit.
Thus, the only card that could be played on a 9♣ would be an 8♣.
No pile may contain more than 13 cards.

Once on any foundation, cards may not be moved back off.
]]
    ACE_FOUNDATIONS = {}
    for x = 1, 4 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileLabel(pile, U[1])
        table.insert(ACE_FOUNDATIONS, pile)
    end
    KING_FOUNDATIONS = {}
    for x = 10, 13 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileLabel(pile, U[13])
        table.insert(KING_FOUNDATIONS, pile)
    end

    -- Twenty-five tableau piles of four cards each, splayed downward. All cards are dealt face up.
    for x = 1, 13 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        for i = 1, 4 do
            MoveCard(Stock.Pile, pile)
        end
    end
    for x = 1.5, 12.5 do
        pile = AddPile("Tableau", x, 5, FAN_DOWN)
        for i = 1, 4 do
            MoveCard(Stock.Pile, pile)
        end
    end

    -- There is one reserve, initially containing four cards.
    -- The top card in the pile is dealt face up, all others are face down.
    -- It is not possible to move any cards into the reserve.
    -- The top card of the reserve can be played to the tableau or the foundation.
    AddPile("Reserve", 7, 1, FAN_RIGHT)
    for i = 1, 4 do
        card = MoveCard(Stock.Pile, Reserve.Pile)
        CardProne(card, true)
    end
    CardProne(Last(Reserve.Pile), false)

    if not Empty(Stock.Pile) then
        io.stderr:write("Oops - there are still " .. Len(Stock.Pile) .. " cards in the Stock\n")
    end
end

-- TailMoveError constraints (_Tableau only)

function Tableau.TailMoveError(tail)
    -- Only one card may moved at a time, never sequences.
    if Len(tail) > 1 then
        return "Can only move a single card"
    end
    return nil
end

-- TailAppendError constraints

function Foundation.TailAppendError(pile, tail)
    if table.contains(ACE_FOUNDATIONS, pile) then
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
    elseif table.contains(KING_FOUNDATIONS, pile) then
        if Empty(pile) then
            local c1 = First(tail)
            if CardOrdinal(c1) ~= 13 then
                return "Foundation can only accept a King, not a " .. V[CardOrdinal(c1)]
            end
        else
            local c1 = Last(pile)
            local c2 = First(tail)
            local err = DownSuit(c1, c2) if err then return err end
        end
    else
        io.stderr:write("Oops - is the pile not aFoundation?\n")
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    if Empty(pile) then
        -- do nothing, empty accept any card
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = PlusMinusOneSuitWrap(c1, c2) if err then return err end
    end
    return nil
end

-- PileConformantError (Tableau only)

function Tableau.PileConformantError(pile)
    local c1 = First(pile)
    for i = 2, Len(pile) do
        local c2 = Get(pile, n)
        local err = PlusMinusOneSuitWrap(c1, c2) if err then return err end
        c1 = c2
    end
    return nil
end

-- SortedAndUnSorted (Tableau only)

function Tableau.SortedAndUnsorted(pile)
    local sorted = 0
    local unsorted = 0
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = PlusMinusOneSuitWrap(c1, c2)
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

function AfterMove()
    -- Empty spaces in the tableau are automatically filled with a card from the reserve.
    -- If the reserve is empty, then empty spaces in the tableau may be filled by any card.
    if Len(Reserve.Pile) > 0 then
        for _, pile in ipairs(Tableau.Piles) do
            if Empty(pile) then
                MoveCard(Reserve.Pile, pile)
            end
        end
    end
end
