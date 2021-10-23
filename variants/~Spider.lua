-- Spider

V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}

POWERMOVES = false

function BuildPiles()

    STOCK = AddPile("Stock", 1, 1, FAN_NONE, PACKS, SUITS)
    SetPileRecycles(STOCK, 0)

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
          SetCardProne(c, true)
        end
        MoveCard(STOCK, pile)
    end
    for x = 5, 10 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for n=1,3 do
          local c = MoveCard(STOCK, pile)
          SetCardProne(c, true)
        end
        MoveCard(STOCK, pile)
    end
end

--[[
function CanTailBeMoved_Discard(tail)
    return false, "Cannot move cards from a Discard"
end
]]

function CanTailBeMoved_Tableau(tail)
    local c1 = TailGet(tail, 1)
    for i = 2, TailLen(tail) do
        local c2 = TailGet(tail, i)

        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Moved cards must all have the same suit"
        end
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false, "Moved cards must be in descending order"
        end

        c1 = c2
    end
    return true
end

function CanTailBeAppended_Discard(pile, tail)
    if TailLen(tail) ~= 13 then
        return false, "Discard can only accept 13 cards"
    else
        local c1 = TailGet(tail, 1)
        if CardOrdinal(c1) ~= 13 then
            return false, "Can only discard from a 13, not a " .. V[CardOrdinal(c1)]
        end
        for i = 2, TailLen(tail) do
            local c2 = TailGet(tail, i)
            if CardSuit(c1) ~= CardSuit(c2) then
                return false, "Discarded piles must be all the same suit"
            end
            if CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
                return false, "Discarded piles build up"
            end
            c1 = c2
        end
    end
    return true
end

function CanTailBeAppended_Tableau(pile, tail)
    if PileLen(pile) == 0 then
        -- accept any card
    else
        local c1 = PilePeek(pile)
        local c2 = TailGet(tail, 1)
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false, "Tableaux build down"
        end
    end
    return true
end

function IsPileConformant_Discard(pile)
    local c1 = PilePeek(pile)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, n)
        if CardSuit(c1) ~= CardSuit(c2) then
            return false
        end
        if CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
            return false
        end
        c1 = c2
    end
    return true
end

function IsPileConformant_Tableau(pile)
    return IsPileConformant_Discard(pile)
end

function SortedAndUnsorted_Tableau(pile)
    local sorted = 0
    local unsorted = 0
    local c1 = PileGet(pile, 1)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, i)
        if CardSuit(c1) ~= CardSuit(c2) then
            unsorted = unsorted + 1
        elseif CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            unsorted = unsorted + 1
        else
            sorted = sorted + 1
        end
        c1 = c2
    end
    return sorted, unsorted
end

function Tapped_Stock(tail)

    if not tail then
        return "No more cards in Stock"
    end

    local errMsg = nil
    local tabCards = 0
    local emptyTabs = 0

    for _, tab in ipairs(TABLEAUX) do
        if PileLen(tab) == 0 then
            emptyTabs = emptyTabs + 1
        else
            tabCards = tabCards + PileLen(tab)
        end
    end
    if emptyTabs > 0 and tabCards >= #TABLEAUX then
        errMsg = "All empty tableaux must be filled before dealing a new row"
    else
        for _, tab in ipairs(TABLEAUX) do
            MoveCard(STOCK, tab)
        end
    end

    return errMsg
end
