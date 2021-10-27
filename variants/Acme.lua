-- Acme

--[[
  https://en.wikipedia.org/wiki/Acme_(solitaire)

  Acme is a Canfield type of patience or solitaire card game using a single deck of playing cards.
  Acme has four tableau stacks of one card each, and they are built down in suit.
  There are also four Foundations that build up in suit.
  The Reserve Pile contains 13 cards which can be played onto the Foundations or Tableau Stacks.
  The deck turns up one card at a time.
  Only the top card of a Tableau stack can be moved.
  These cards can be moved to a Foundation or onto another Tableau stack.
  The Tableau builds down in suit, and the Foundations build up in suit.
  Cards from the Reserve automatically fill empty spaces.
  Any card can fill empty Tableau spaces after the Reserve is empty.
  There is only one redeal allowed in this game, so only two passes through the deck are allowed.
  Strategy: Rather than using the cards from the deck, a player should try to use all of the reserve cards first.
  Only two passes are allowed, so the deck should be used wisely.
]]

V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}
POWER_MOVES = false
SEED=12620
STOCK_DEAL_CARDS = 1

function BuildPiles()

    STOCK = AddPile("Stock", 1, 1, FAN_NONE, 1, 4)
    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)
    
    local pile

    FOUNDATIONS = {}
    for x = 4, 7 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileAccept(pile, 1)
        table.insert(FOUNDATIONS, pile)
    end

    RESERVES = {}
    pile = AddPile("Reserve", 1, 2, FAN_DOWN)
    table.insert(RESERVES, pile)
    for n = 1, 13 do
        local c = MoveCard(STOCK, pile)
        CardProne(c, true)
    end
    CardProne(PilePeek(pile), false)

    TABLEAUX = {}
    for x = 4, 7 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        MoveCard(STOCK, pile)
    end

end

function StartGame()
    STOCK_RECYCLES = 1
end

-- CanTailBeMoved constraints (Tableau only)

function Tableau.CanTailBeMoved(tail)
    local c1 = TailGet(tail, 1)
    if POWER_MOVES then
        for i = 2, TailLen(tail) do
            local c2 = TailGet(tail, i)
            if CardSuit(c1) ~= CardSuit(c2) then
                return false, "Moved cards must be built in suit"
            end
            if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
                return false, "Moved cards must be in descending rank"
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
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false, "Tableaux build down"
        end
    end
    return true
end

-- IsPileConformant

-- function Foundation.IsPileConformant(pile)
--     local c1 = PilePeek(pile)
--     for i = 2, PileLen(pile) do
--         local c2 = PileGet(pile, n)
--         if CardSuit(c1) ~= CardSuit(c2) then
--             return false, "Foundations must be built in suit"
--         end
--         if CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
--             return false, "Foundations build up"
--         end
--         c1 = c2
--     end
--     return true
-- end

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
        elseif CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            unsorted = unsorted + 1
        else
            sorted = sorted + 1
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

function AfterMove()
    for i = 1, 4 do
        if PileLen(TABLEAUX[i]) == 0 then
            MoveCard(RESERVES[1], TABLEAUX[i])
        end
    end
end
