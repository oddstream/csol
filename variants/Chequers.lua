-- Chequers

--[[
    https://politaire.com/chequers
    https://politaire.com/help/chequers
]]

V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}
POWER_MOVES = false
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

function PlusMinusOneAndWrap(c1, c2)
    if CardOrdinal(c1) == 1 and CardOrdinal(c2) == 13 then
        -- wrap from Ace to King
    elseif CardOrdinal(c1) == 13 and CardOrdinal(c2) == 1 then
        -- wrap from King to Ace
    elseif CardOrdinal(c1) == CardOrdinal(c2) + 1 then
        -- build up one
    elseif CardOrdinal(c1) == CardOrdinal(c2) - 1 then
        -- build down one
    else
        return false
    end
    return true
end

-- C sets variables 'BAIZE', 'STOCK', FAN_*

function BuildPiles()

    STOCK = AddPile("Stock", -5, -5, FAN_NONE, 2, 4)

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
        PileAccept(pile, 1)
        table.insert(ACE_FOUNDATIONS, pile)
    end
    KING_FOUNDATIONS = {}
    for x = 10, 13 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileAccept(pile, 13)
        table.insert(KING_FOUNDATIONS, pile)
    end

    -- Twenty-five tableau piles of four cards each, splayed downward. All cards are dealt face up.
    TABLEAUX = {}
    for x = 1, 13 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for i = 1, 4 do
            MoveCard(STOCK, pile)
        end
    end
    for x = 1.5, 12.5 do
        pile = AddPile("Tableau", x, 5, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for i = 1, 4 do
            MoveCard(STOCK, pile)
        end
    end

    -- There is one reserve, initially containing four cards.
    -- The top card in the pile is dealt face up, all others are face down.
    -- It is not possible to move any cards into the reserve.
    -- The top card of the reserve can be played to the tableau or the foundation.
    RESERVE = AddPile("Reserve", 7, 1, FAN_RIGHT)
    for i = 1, 4 do
        card = MoveCard(STOCK, RESERVE)
        CardProne(card, true)
    end
    CardProne(PilePeek(RESERVE), false)

    if PileLen(STOCK) > 0 then
        io.stderr:write("Oops - there are still " .. PileLen(STOCK) .. " cards in the Stock\n")
    end
end

-- CanTailBeMoved constraints (_Tableau only)

function Tableau.CanTailBeMoved(tail)
    -- Only one card may moved at a time, never sequences.
    if TailLen(tail) > 1 then
        return false, "Can only move a single card"
    end
    return true
end

-- CanTailBeAppended constraints

function Foundation.CanTailBeAppended(pile, tail)
    if table.contains(ACE_FOUNDATIONS, pile) then
        if PileLen(pile) == 0 then
            local c1 = TailGet(tail, 1)
            if CardOrdinal(c1) ~= 1 then
                return false, "Foundation can only accept an Ace, not a " .. V[CardOrdinal(c1)]
            end
        else
            local c1 = PilePeek(pile)
            local c2 = TailGet(tail, 1)
            if CardSuit(c1) ~= CardSuit(c2) then
                return false, "Foundations must be built in suit"
            end
            if CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
                return false, "Foundations build up"
            end
        end
    elseif table.contains(KING_FOUNDATIONS, pile) then
        if PileLen(pile) == 0 then
            local c1 = TailGet(tail, 1)
            if CardOrdinal(c1) ~= 13 then
                return false, "Foundation can only accept a King, not a " .. V[CardOrdinal(c1)]
            end
        else
            local c1 = PilePeek(pile)
            local c2 = TailGet(tail, 1)
            if CardSuit(c1) ~= CardSuit(c2) then
                return false, "Foundations must be built in suit"
            end
            if CardOrdinal(c1) - 1 ~= CardOrdinal(c2) then
                return false, "Foundations build down"
            end
        end
    else
        io.stderr:write("Oops - is the pile not aFoundation?\n")
    end
    return true
end

function Tableau.CanTailBeAppended(pile, tail)
    if PileLen(pile) == 0 then
        -- do nothing, empty accept any card
    else
        local c1 = PilePeek(pile)
        local c2 = TailGet(tail, 1)
        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Tableaux build in suit"
        end
        if not PlusMinusOneAndWrap(c1, c2) then
            return false, "Tableaux build up or down by one"
        end
    end
    return true
end

-- IsPileConformant (Tableau only)

function Tableau.IsPileConformant(pile)
    local c1 = PilePeek(pile)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, n)
        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Tableaux build in suit"
        end
        if not PlusMinusOneAndWrap(c1, c2) then
            return false, "Tableaux build up or down by one"
        end
        c1 = c2
    end
    return true
end

-- SortedAndUnSorted (Tableau only)

function Tableau.SortedAndUnsorted(pile)
    local sorted = 0
    local unsorted = 0
    local c1 = PileGet(pile, 1)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, i)
        if CardSuit(c1) ~= CardSuit(c2) then
            unsorted = unsorted + 1
        elseif PlusMinusOneAndWrap(c1, c2) then
            sorted = sorted + 1
        else
            unsorted = unsorted + 1
        end
        c1 = c2
    end
    return sorted, unsorted
end

-- Actions

function AfterMove()
    -- Empty spaces in the tableau are automatically filled with a card from the reserve.
    -- If the reserve is empty, then empty spaces in the tableau may be filled by any card.
    if PileLen(RESERVE) > 0 then
        for _, pile in ipairs(TABLEAUX) do
            if PileLen(pile) == 0 then
                MoveCard(RESERVE, pile)
            end
        end
    end
end
