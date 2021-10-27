-- American Toad

--[[
American Toad is a solitaire game using two decks of playing cards.
This game is similar to Canfield except that the tableau builds down in suit, and a partial tableau stack cannot be moved (only the top card or entire stack can be moved).
The object of the game is to move all cards to the foundations.
American Toad has eight tableau stacks. Each tableau stack contains one card and builds down in suit wrapping from Ace to King, e.g. 3♠, 2♠, A♠, K♠...
There are also eight foundations that build up in suit, e.g. 7♦, 8♦, 9♦...
The game includes one reserve pile with twenty cards that can be played onto the tableau or foundations.
There is a deck usually at the bottom right that turns up one card at a time.
One card is dealt onto the first foundation. This rank will be used as a base for the other foundations.
The foundations build up in suit, wrapping from King to Ace as necessary. 
]]

V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}
POWER_MOVES = false
STOCK_DEAL_CARDS = 1

function BuildPiles()

    STOCK = AddPile("Stock", 1, 1, FAN_NONE, 2, 4)
    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)

    local pile

    RESERVES = {}
    pile = AddPile("Reserve", 4, 1, FAN_RIGHT)
    table.insert(RESERVES, pile)
    for n = 1, 20 do
        local c = MoveCard(STOCK, pile)
        CardProne(c, true)
    end
    CardProne(PilePeek(pile), false)
    
    FOUNDATIONS = {}
    for x = 1, 8 do
        pile = AddPile("Foundation", x, 2, FAN_NONE)
        table.insert(FOUNDATIONS, pile)
    end

    TABLEAUX = {}
    for x = 1, 8 do
        pile = AddPile("Tableau", x, 3, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        MoveCard(STOCK, pile)
    end

end

function StartGame()
    STOCK_RECYCLES = 1
    MoveCard(STOCK, FOUNDATIONS[1])
    local c = PilePeek(FOUNDATIONS[1])
    for _, pile in ipairs(FOUNDATIONS) do
        PileAccept(pile, CardOrdinal(c))
    end
end

-- CanTailBeMoved constraints (Tableau only)

function Tableau.CanTailBeMoved(tail)
    if TailLen(tail) == 1 then
        return true
    end
    local c1 = TailGet(tail, 1)
    if TailLen(tail) ~= PileLen(CardOwner(c1)) then
        return false, "Can only move one card, or the whole pile"
    else
        for i = 2, TailLen(tail) do
            local c2 = TailGet(tail, i)
            if CardSuit(c1) ~= CardSuit(c2) then
                return false, "Moved cards must be all be the same suit"
            end
            if CardOrdinal(c1) == 1 and CardOrdinal(c2) == 13 then
                -- wrap from Ace to King
            elseif CardOrdinal(c1) == CardOrdinal(c2) + 1 then
                -- down, eg 2 on a 3
            else
                return false, "Moved cards must descend in rank"
            end 
            c1 = c2
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
    if PileLen(pile) == 0 then
        local c1 = TailGet(tail, 1)
        if CardOrdinal(c1) ~= PileAccept(pile) then
            return false, "An empty Foundation can only accept a " .. V[PileAccept(pile)] .. " not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = PilePeek(pile)
        local c2 = TailGet(tail, 1)
        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Foundations must be built in suit"
        end
        if CardOrdinal(c1) == 13 and CardOrdinal(c2) == 1 then
            -- wrap from King to Ace
        elseif CardOrdinal(c1) + 1 == CardOrdinal(c2) then
            -- up, eg 3 on a 2
        else
            return false, "Foundations build up"
        end 
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
            return false, "Tableaux must be built in suit"
        end
        if CardOrdinal(c1) == 1 and CardOrdinal(c2) == 13 then
            -- wrap from Ace to King
        elseif CardOrdinal(c1) == CardOrdinal(c2) + 1 then
            -- down, eg 2 on a 3
        else
            return false, "Tableaux build down in rank"
        end 
    end
    return true
end

-- IsPileConformant

function Tableau.IsPileConformant(pile)
    local c1 = PilePeek(pile)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, i)
        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Tableaux must be built in suit"
        end
        if CardOrdinal(c1) == 1 and CardOrdinal(c2) == 13 then
            -- wrap from Ace to King
        elseif CardOrdinal(c1) == CardOrdinal(c2) + 1 then
            -- down, eg 2 on a 3
        else
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
        elseif CardOrdinal(c1) == 1 and CardOrdinal(c2) == 13 then
            -- wrap from Ace to King
            sorted = sorted + 1
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
