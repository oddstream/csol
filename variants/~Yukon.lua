-- Yukon

-- https://en.wikipedia.org/wiki/Yukon_(solitaire)

dofile("variants/~Library.lua")

STOCK_DEAL_CARDS = 1
STOCK_RECYCLES = 0

function BuildPiles()

    STOCK = AddPile("Stock", -5, -5, FAN_NONE, 1, 4)
    
    local pile
    local card

    FOUNDATIONS = {}
    for y = 1, 4 do
        pile = AddPile("Foundation", 8.5, y, FAN_NONE)
        PileAccept(pile, 1)
        table.insert(FOUNDATIONS, pile)
    end

    TABLEAUX = {}
    pile = AddPile("Tableau", 1, 1, FAN_DOWN)
    if not RELAXED then
        PileAccept(pile, 13)
    end
    table.insert(TABLEAUX, pile)
    card = MoveCard(STOCK, pile)

    local dealDown = 1
    local dealUp = 5
    for x = 2, 7 do
        pile = AddPile("Tableau", x, 1, FAN_DOWN)
        if not RELAXED then
            PileAccept(pile, 13)
        end
        table.insert(TABLEAUX, pile)
        for c = 1, dealDown do
            card = MoveCard(STOCK, pile)
            CardProne(card, true)
        end
        for c = 1, dealUp do
            card = MoveCard(STOCK, pile)
            -- CardProne(card, false)
        end
        dealDown = dealDown + 1
    end

    if not Empty(STOCK) then
        io.stderr:write("Oops! There are still " .. Len(STOCK) .. " cards in the Stock\n")
    end
end

function StartGame()
    if RELAXED then
        Toast("Relaxed version - any card may be placed in an empty pile")
    end
end

-- CanTailBeMoved constraints (Tableau only)

function Tableau.CanTailBeMoved(tail)
    return true
end

-- CanTailBeAppended constraints

function Foundation.CanTailBeAppended(pile, tail)
    if Len(pile) == 0 then
        local c1 = Get(tail, 1)
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
        if not RELAXED then
            local c1 = Get(tail, 1)
            if CardOrdinal(c1) ~= 13 then
                return false, "Empty Tableaux can only accept a King, not a " .. V[CardOrdinal(c1)]
            end
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = DownAltColor(c1, c2) if err then return false, err end
    end
    return true
end

-- IsPileConformant

function Tableau.IsPileConformant(pile)
    local c1 = First(pile)
    for i = 2, Len(pile) do
        local c2 = Get(pile, n)
        local err = DownAltColor(c1, c2) if err then return false, err end
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
        local err = DownAltColor(c1, c2)
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
