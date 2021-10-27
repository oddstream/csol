-- Simple Simon

--[[
    https://en.wikipedia.org/wiki/Simple_Simon_(solitaire)
]]

V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}
POWER_MOVES = false
-- SEED=32808
-- SEED=59517

function BuildPiles()

    STOCK = AddPile("Stock", -5, -5, FAN_NONE, 1, 4)    -- hidden off screen
    
    local pile

    DISCARDS = {}
    for x = 4, 7 do
        pile = AddPile("Discard", x, 1, FAN_NONE)
        table.insert(DISCARDS, pile)
    end

    TABLEAUX = {}
    for x = 1, 3 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for n = 1, 8 do
            MoveCard(STOCK, pile)
        end
    end
    local deal = 7
    for x = 4, 10 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for n = 1, deal do
          local c = MoveCard(STOCK, pile)
        end
        deal = deal - 1
    end

    if PileLen(STOCK) ~= 0 then
        io.stdout:write("Oops, there are " .. PileLen(STOCK) .. " cards still in the Stock\n")
    end
end

-- function StartGame()
-- end

-- CanTailBeMoved constraints (Tableau only)

function Tableau.CanTailBeMoved(tail)
    -- A sequence of cards, decrementing in rank and of the same suit, can be moved as one
    local c1 = TailGet(tail, 1)
    for i = 2, TailLen(tail) do
        local c2 = TailGet(tail, i)
        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Moved cards must all the the same suit"
        end
        -- K Q J 10 9 .. 2 1
        if CardOrdinal(c1) - 1 ~= CardOrdinal(c2) then
            return false, "Moved cards must descend in rank"
        end
        c1 = c2
    end
    return true
end

-- CanTailBeAppended constraints

function Discard.CanTailBeAppended(pile, tail)
    -- C will have checked that there are (13 - number of cards in a suit) cards in the tail

    local c1 = TailGet(tail, 1)
    if CardOrdinal(c1) ~= 13 then
        return false, "Can only discard from a King, not a " .. V[CardOrdinal(c1)]
    end
    for i = 2, TailLen(tail) do
        local c2 = TailGet(tail, i)
        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Discarded piles must be all the same suit"
        end
        -- K Q J 10 9 .. 2 1
        if CardOrdinal(c1) - 1 ~= CardOrdinal(c2) then
            return false, "Discarded piles must build down in rank"
        end
        c1 = c2
    end
    return true
end

function Tableau.CanTailBeAppended(pile, tail)
    -- A card can be placed on any card on the top of a column whose rank is greater than it by one (with no cards that can be placed above an Ace). 
    if PileLen(pile) == 0 then
        -- An empty column may be filled by any card
    else
        local c1 = PilePeek(pile)
        local c2 = TailGet(tail, 1)
            -- K Q J 10 9 .. 2 1
        if CardOrdinal(c1) - 1 ~= CardOrdinal(c2) then
            return false, "Tableaux build down in rank"
        end
    end
    return true
end

-- IsPileConformant

function Tableau.IsPileConformant(pile)
    local c1 = PilePeek(pile)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, n)
        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Tableaux build in suit"
        end
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false, "Tableaux build down in rank"
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
        elseif CardOrdinal(c1) == CardOrdinal(c2) + 1 then
            sorted = sorted + 1
        else
            unsorted = unsorted + 1
        end
        c1 = c2
    end
    return sorted, unsorted
end

-- Actions
