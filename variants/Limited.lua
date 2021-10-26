-- Limited

V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}
POWER_MOVES = true
SEED = 4  -- winnable
STOCK_DEAL_CARDS = 1
STOCK_RECYCLES = 32767

-- C sets variables 'BAIZE', 'STOCK', FAN_*

function BuildPiles()

    STOCK = AddPile("Stock", 1, 1, FAN_NONE, 2, 4)

    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)

    local pile

    FOUNDATIONS = {}
    for x = 5, 12 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileAccept(pile, 1)
        table.insert(FOUNDATIONS, pile)
    end

    TABLEAUX = {}
    for x = 1, 12 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for n = 1, 3 do
            MoveCard(STOCK, pile)
        end
    end

end

-- CanTailBeMoved constraints (_Tableau only)

function Tableau.CanTailBeMoved(tail)
    if POWER_MOVES then
        local c1 = TailGet(tail, 1)
        for i = 2, TailLen(tail) do
            local c2 = TailGet(tail, i)
            if CardSuit(c1) ~= CardSuit(c2) then
                return false
            end
            if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
                return false
            end
            c1 = c2
        end
    else
        if TailLen(tail) > 1 then
            return false, "Can only move a single card"
        end
    end
    return true
end

-- CanTailBeAppended constraints

function Waste.CanTailBeAppended(pile, tail)
    if CardOwner(TailGet(tail, 1)) ~= STOCK then
        return false, "The Waste can only accept cards from the Stock"
    end
    return true
end

function Foundation.CanTailBeAppended(pile, tail)
    if TailLen(tail) > 1 then
        return false, "Foundations can only accept a single card"
    else
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
    end
    return true
end

function Tableau.CanTailBeAppended(pile, tail)
    if PileLen(pile) == 0 then
        -- do nothing, empty accept any card
    else
        local c1 = PilePeek(pile)
        for i = 1, TailLen(tail) do
            local c2 = TailGet(tail, i)
            if not c2 then
                io.stderr:write("CanTailBeAppended: nil tail card at index " .. i .. "\n")
                break
            end
            if CardSuit(c1) ~= CardSuit(c2) then
                return false, "Tableaux build in suit"
            end
            if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
                return false, "Tableaux build down"
            end
            c1 = c2
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
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false, "Tableaux build down"
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

function Stock.Tapped(tail)
    if tail == nil then
        if STOCK_RECYCLES == 0 then
            Toast("No more Stock recycles")
            return
        end
        if PileLen(WASTE) > 0 then
            while PileLen(WASTE) > 0 do
                MoveCard(WASTE, STOCK)
            end
            STOCK_RECYCLES = STOCK_RECYCLES - 1
          end
      else
        for i = 1, STOCK_DEAL_CARDS do
            MoveCard(STOCK, WASTE)
        end
    end
end
