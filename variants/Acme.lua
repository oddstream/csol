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
POWERMOVES = false
SEED=12620
STOCKDEALCARDS = 1

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

    RESERVES = {}
    pile = AddPile("Reserve", 1, 2, FAN_DOWN)
    table.insert(RESERVES, pile)
    for n = 1, 13 do
        local c = MoveCard(STOCK, pile)
        SetCardProne(c, true)
    end
    SetCardProne(PilePeek(pile), false)

    TABLEAUX = {}
    for x = 4, 7 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        MoveCard(STOCK, pile)
    end

end

function StartGame()
  STOCK_RECYCLES = 1
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
    if POWERMOVES then
        for i = 2, TailLen(tail) do
            local c2 = TailGet(tail, i)
            if CardSuit(c1) ~= CardSuit(c2) then
                return false, "Moved cards must be built in suit"
            end
            if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
                return false, "Moved cards must be in increasing value"
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

function CanTailBeAppended_Waste(pile, tail)
    if CardOwner(TailGet(tail, 1)) ~= STOCK then
        return false, "The Waste can only accept cards from the Stock"
    end
    return true
end

function CanTailBeAppended_Foundation(pile, tail)
    if TailLen(tail) > 1 then
        return false, "Foundation can only accept a single card"
    else
        if PileLen(pile) == 0 then
            local c1 = TailGet(tail, 1)
            if CardOrdinal(c1) ~= 1 then
                return false, "Foundation can only accept a 1, not a " .. V[CardOrdinal(c1)]
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

function CanTailBeAppended_Tableau(pile, tail)
    if PileLen(pile) == 0 then
        -- do nothing, empty accept any card
    else
        local c1 = PilePeek(pile)
        for i = 1, TailLen(tail) do
            -- io.stderr:write(i .. " of " .. TailLen(tail) .. "\n")
            local c2 = TailGet(tail, i)
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

-- IsPileConformant

function IsPileConformant_Foundation(pile)
    local c1 = PilePeek(pile)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, n)
        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Foundations must be built in suit"
        end
        if CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
            return false, "Foundations build up"
        end
        c1 = c2
    end
    return true
end

function IsPileConformant_Tableau(pile)
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

-- SortedAndUnSorted (_Tableau only)

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

-- Actions

function Tapped_Stock(tail)
    if tail == nil then
        if STOCK_RECYCLES == 0 then
            return "No more Stock recycles"
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

function AfterMove()
    for i = 1, 4 do
        if PileLen(TABLEAUX[i]) == 0 then
            MoveCard(RESERVES[1], TABLEAUX[i])
        end
    end
end
