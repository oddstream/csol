-- Sir Tommy, Old Patience, Try Again

--[[
  https://en.wikipedia.org/wiki/Sir_Tommy

  Cards are dealt one at a time.
  When an ace turns up, it forms a foundation which builds up to King regardless of suit.
  Four such foundations should be built.
  A card that cannot yet be placed on the foundation is placed onto one of four wastepiles; 
  once placed, it cannot be moved, but the top card of each wastepile remains available 
  to be placed on a foundation.

  The game is won if all cards are emptied from the wastepiles and built on the foundations.
]]

V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}

STOCK_DEAL_CARDS = 1
STOCK_RECYCLES = 0

function BuildPiles()

    STOCK = AddPile("Stock", 1, 1, FAN_NONE, 1, 4)

    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)
    
    local pile

    FOUNDATIONS = {}
    for x = 4, 7 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        table.insert(FOUNDATIONS, pile)
    end
    FOUNDATION_ACCEPT = 1

    TABLEAUX = {}
    for x = 4, 7 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
    end

end

function StartGame()
end

-- CanTailBeMoved constraints (_Tableau only)

--[[
function CanTailBeMoved_Waste(tail)
    if TailLen(tail) > 1 then
        return false, "Only a single card can be moved from the Waste"
    end
    return true
end

function CanTailBeMoved_Foundation(tail)
    return false, "Cannot move cards from a Foundation"
end
]]

function CanTailBeMoved_Tableau(tail)
    local c1 = TailGet(tail, 1)
    if TailLen(tail) > 1 then
        return false, "Can only move a single card"
    end
    return true
end

-- CanTailBeAppended constraints

function CanTailBeAppended_Waste(pile, tail)
    if PileLen(pile) > 0 then
        return false, "The Waste already contains a card"
    end
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
        return false, "Foundation can only accept a single card"
    else
        if PileLen(pile) == 0 then
            local c1 = TailGet(tail, 1)
            if CardOrdinal(c1) ~= 1 then
                return false, "Foundation can only accept an Ace, not a " .. V[CardOrdinal(c1)]
            end
        else
            -- build up regardless of suit
            local c1 = PilePeek(pile)
            local c2 = TailGet(tail, 1)
            if CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
                return false, "Foundations build up"
            end
        end
    end
    return true
end

function CanTailBeAppended_Tableau(pile, tail)
    local c1 = PilePeek(pile)
    local c2 = TailGet(tail, 1)
    if CardOwner(c2) ~= WASTE then
        return false, "Cards can only be moved here from the Waste"
    end
    return true
end

-- IsPileConformant (_Tableau only)

-- SortedAndUnSorted (_Tableau only)

function SortedAndUnsorted_Tableau(pile)
    return 0, PileLen(pile)
end

-- Actions

function Tapped_Stock(tail)
    -- only allow one card at a time in waste
    if PileLen(WASTE) == 0 then
        MoveCard(STOCK, WASTE)
    else
        return "The Waste must be emptied first"
    end
end
