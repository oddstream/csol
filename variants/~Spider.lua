-- Spider

dofile("variants/~Library.lua")

POWER_MOVES = false
STOCK_RECYCLES = 0

function BuildPiles()

    STOCK = AddPile("Stock", 1, 1, FAN_NONE, PACKS, SUITS)

    local pile

    DISCARDS = {}
    for x = 3, 10 do
        pile = AddPile("Discard", x, 1, FAN_NONE)
        table.insert(DISCARDS, pile)
    end

    TABLEAUX = {}
    for x = 1, 4 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for n=1,4 do
          local c = MoveCard(STOCK, pile)
          CardProne(c, true)
        end
        MoveCard(STOCK, pile)
    end
    for x = 5, 10 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for n=1,3 do
          local c = MoveCard(STOCK, pile)
          CardProne(c, true)
        end
        MoveCard(STOCK, pile)
    end
end

function Tableau.CanTailBeMoved(tail)
    local c1 = Get(tail, 1)
    for i = 2, Len(tail) do
        local c2 = Get(tail, i)
        local err = DownSuit(c1, c2) if err then return false, err end
        c1 = c2
    end
    return true
end

function Discard.CanTailBeAppended(pile, tail)
    if Len(tail) ~= 13 then
        return false, "Discard can only accept 13 cards"
    else
        local c1 = First(tail)
        if CardOrdinal(c1) ~= 13 then
            return false, "Can only discard from a 13, not a " .. V[CardOrdinal(c1)]
        end
        for i = 2, Len(tail) do
            local c2 = Get(tail, i)
            local err = DownSuit(c1, c2) if err then return false, err end
            c1 = c2
        end
    end
    return true
end

function Tableau.CanTailBeAppended(pile, tail)
    if Len(pile) == 0 then
        -- accept any card
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false, "Tableaux build down"
        end
    end
    return true
end

function Discard.IsPileConformant(pile)
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, n)
        local err = DownSuit(c1, c2) if err then return false end
        c1 = c2
    end
    return true
end

function Tableau.IsPileConformant(pile)
    return Discard.IsPileConformant(pile)
end

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

function Stock.Tapped(tail)

    if not tail then
        Toast("No more cards in Stock")
        return
    end

    local errMsg = nil
    local tabCards = 0
    local emptyTabs = 0

    for _, tab in ipairs(TABLEAUX) do
        if Len(tab) == 0 then
            emptyTabs = emptyTabs + 1
        else
            tabCards = tabCards + Len(tab)
        end
    end
    if emptyTabs > 0 and tabCards >= #TABLEAUX then
        Toast("All empty tableaux must be filled before dealing a new row")
    else
        for _, tab in ipairs(TABLEAUX) do
            MoveCard(STOCK, tab)
        end
    end
end
