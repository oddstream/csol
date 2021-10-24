-- Yukon

V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}
STOCK_DEAL_CARDS = 1
STOCK_RECYCLES = 0

function BuildPiles()

    STOCK = AddPile("Stock", -5, -5, FAN_NONE, 1, 4)
    
    local pile
    local card

    FOUNDATIONS = {}
    for y = 1, 4 do
        pile = AddPile("Foundation", 8.5, y, FAN_NONE)
        table.insert(FOUNDATIONS, pile)
    end
    FOUNDATION_ACCEPT = 1

    TABLEAUX = {}
    pile = AddPile("Tableau", 1, 1, FAN_DOWN)
    table.insert(TABLEAUX, pile)
    card = MoveCard(STOCK, pile)

    local dealDown = 1
    local dealUp = 5
    for x = 2, 7 do
        pile = AddPile("Tableau", x, 1, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for c = 1, dealDown do
            card = MoveCard(STOCK, pile)
            SetCardProne(card, true)
        end
        for c = 1, dealUp do
            card = MoveCard(STOCK, pile)
            SetCardProne(card, false)
        end
        dealDown = dealDown + 1
    end
    TABLEAU_ACCEPT = 13

    if PileLen(STOCK) > 0 then
        io.stderr:write("Oops! There are still " .. PileLen(STOCK) .. " cards in the Stock\n")
    end
end

-- CanTailBeMoved constraints (_Tableau only)

function CanTailBeMoved_Tableau(tail)
    return true
end

-- CanTailBeAppended constraints

function CanTailBeAppended_Foundation(pile, tail)
    if TailLen(tail) > 1 then
        return false, "Foundations can only accept a single card"
    elseif PileLen(pile) == 0 then
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
    return true
end

function CanTailBeAppended_Tableau(pile, tail)
    if PileLen(pile) == 0 then
        local c1 = TailGet(tail, 1)
        if CardOrdinal(c1) ~= 13 then
            return false, "Empty Tableaux can only accept a King, not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = PilePeek(pile)
        local c2 = TailGet(tail, 1)
        if CardColor(c1) == CardColor(c2) then
            return false, "Tableaux build in alternating color"
        end
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false, "Tableaux build down"
        end
    end
    return true
end

-- IsPileConformant

function IsPileConformant_Tableau(pile)
    local c1 = PilePeek(pile)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, n)
        if CardColor(c1) == CardColor(c2) then
            return false, "Tableaux build in alternating color"
        end
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false, "Tableaux build down"
        end
        c1 = c2
    end
    return true
end

-- SortedAndUnSorted (_Tableau only)

function SortedAndUnsorted_Tableau(pile)
    local sorted = 0
    local unsorted = 0
    local c1 = PileGet(pile, 1)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, i)
        if CardColor(c1) == CardColor(c2) then
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
