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
PACKS = 2
SUITS = 4
POWERMOVES = false

-- C sets variables 'BAIZE', 'STOCK', FAN_*

function Build()

    -- a stock pile is always created first, and filled with PACKS of shuffled cards
    PileMoveTo(STOCK, 1, 1)

    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)

    RESERVE = AddPile("Reserve", 4, 1, FAN_RIGHT)
    for n = 1, 20 do
        local c = MoveCard(STOCK, RESERVE)
        SetCardProne(c, true)
    end
    SetCardProne(PilePeek(RESERVE), false)
    
    FOUNDATIONS = {}
    for x = 1, 8 do
        local pile = AddPile("Foundation", x, 2, FAN_NONE)
        -- FOUNDATIONS[#FOUNDATIONS+1] = pile
        table.insert(FOUNDATIONS, pile)
    end

    TABLEAUX = {}
    for x = 1, 8 do
        local pile = AddPile("Tableau", x, 3, FAN_DOWN)
        MoveCard(STOCK, pile)
        table.insert(TABLEAUX, pile)
    end

end

function StartGame()
    STOCK_RECYCLES = 1
    SetPileRecycles(STOCK, STOCK_RECYCLES)
    MoveCard(STOCK, FOUNDATIONS[1])
    local c = PilePeek(FOUNDATIONS[1])
    FOUNDATION_ACCEPT = CardOrdinal(c)
    for i = 1, 8 do
        SetPileAccept(FOUNDATIONS[i], FOUNDATION_ACCEPT)
    end
    MoveCard(STOCK, WASTE)
end

function CanTailBeMoved_Foundation(tail)
    return false, "You cannot move cards from a Foundation"
end

function CanTailBeMoved_Tableau(tail)
    local c1 = TailGet(tail, 1)
    local pile = CardOwner(c1)
    if not (TailLen(tail) == 1 or TailLen(tail) == PileLen(pile)) then
        return false, "Can only move one card, or the whole pile"
    end
    return true
end

function CanTailBeMoved_Waste(tail)
    if TailLen(tail) > 1 then
        return false, "Only one Waste card can be moved"
    end
    return true
end

function CanTailBeMoved_Reserve(tail)
    if TailLen(tail) > 1 then
        return false, "Only one Reserve card can be moved"
    end
    return true
end

function CanTailBeAppended_Foundation(pile, tail)
    if PileLen(pile) == 0 then
        local c1 = TailGet(tail, 1)
        if CardOrdinal(c1) ~= FOUNDATION_ACCEPT then
            return false, "An empty Foundation can only accept a " .. V[FOUNDATION_ACCEPT] .. " not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = PilePeek(pile)
        for i = 1, TailLen(tail) do
            local c2 = TailGet(tail, i)
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
            c1 = c2
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
            local c2 = TailGet(tail, i)
            if CardSuit(c1) ~= CardSuit(c2) then
                return false, "Tableaux must be built in suit"
            end
            if CardOrdinal(c1) == 1 and CardOrdinal(c2) == 13 then
                -- wrap from Ace to King
            elseif CardOrdinal(c1) == CardOrdinal(c2) + 1 then
                -- down, eg 2 on a 3
            else
                return false, "Tableaux build up"
            end 
            c1 = c2
        end
    end
    return true
end

function IsPileConformant_Foundation(pile)
    local c1 = PilePeek(pile)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, i)
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
        c1 = c2
    end
end

function IsPileConformant_Tableau(pile)
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
            return false, "Tableaux build up"
        end 
        c1 = c2
    end
    return true
end

function CardTapped(card)
  if CardOwner(card) == STOCK then
    MoveCard(STOCK, WASTE)
  end
end

function PileTapped(pile)
--   io.stdout:write("PileTapped\n")
  if pile == STOCK then
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
  elseif pile == WASTE then
    if PileLen(STOCK) > 0 then
      MoveCard(STOCK, WASTE)
    end
  end
end
