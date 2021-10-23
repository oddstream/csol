-- Klondike

V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}
POWERMOVES = false
-- SEED = 3 -- 2 winnable draw three
-- SEED = 39880
-- SEED=35495
function BuildPiles()

    STOCK = AddPile("Stock", 1, 1, FAN_NONE, 1, 4)
    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)
    
    local pile

    FOUNDATIONS = {}
    for x = 4, 7 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        table.insert(FOUNDATIONS, pile)
        SetPileAccept(pile, 1)
    end

    TABLEAUX = {}
    local deal = 1
    for x = 1, 7 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        SetPileAccept(pile, 13)
        for n = 1, deal do
          local c = MoveCard(STOCK, pile)
          SetCardProne(c, true)
        end
        SetCardProne(PilePeek(pile), false)
        deal = deal + 1
    end

end

function StartGame()
    STOCK_RECYCLES = 999
    SetPileRecycles(STOCK, STOCK_RECYCLES)
end

-- CanTailBeMoved constraints (_Tableau only)

--[[
function CanTailBeMoved_Waste(tail)
    if TailLen(tail) > 1 then
        return false, "Only a single card can be moved from the Waste"
    end
    return true
end
]]

--[[
function CanTailBeMoved_Foundation(tail)
    return false, "Cannot move cards from a Foundation"
end
]]

function CanTailBeMoved_Tableau(tail)
    local c1 = TailGet(tail, 1)
    for i = 2, TailLen(tail) do
        local c2 = TailGet(tail, i)
        if CardColor(c1) == CardColor(c2) then
            return false, "Card must be in alternating colors"
        end
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false, "Cards must be in increasing value"
        end
        c1 = c2
    end
    return true
end

-- CanTailBeAppended constraints

function CanTailBeAppended_Waste(pile, tail)
    if TailLen(tail) > 1 then
        return false, "The Waste can only accept a single card"
    end
    if CardOwner(TailGet(tail, 1)) ~= STOCK then
        return false, "The Waste can only accept cards from the Stock"
    end
    return true
end

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
        for i = 1, TailLen(tail) do
            local c2 = TailGet(tail, i)
            if CardColor(c1) == CardColor(c2) then
                return false, "Tableaux build in alternate color"
            end
            if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
                return false, "Tableaux build down"
            end
            c1 = c2
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
            return false, "Tableaux build in alternate color"
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
        if CardColor(c1) ~= CardColor(c2) then
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

function Tapped_Stock(tail)
    if tail == nil then
        if STOCK_RECYCLES == 0 then
            return "No more Stock recycles"
        elseif 1 == STOCK_RECYCLES then
            Toast("Last Stock recycle")
        elseif 2 == STOCK_RECYCLES then
            Toast("One Stock recycle remaining")
        end
        if PileLen(WASTE) > 0 then
            while PileLen(WASTE) > 0 do
                MoveCard(WASTE, STOCK)
            end
            STOCK_RECYCLES = STOCK_RECYCLES - 1
            SetPileRecycles(STOCK, STOCK_RECYCLES)
        end
    else
        for i = 1, STOCKDEALCARDS do
            MoveCard(STOCK, WASTE)
        end
    end
end
