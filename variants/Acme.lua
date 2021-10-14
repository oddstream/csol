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
PACKS = 1
SUITS = 4
POWERMOVES = false
-- SEED = 3 -- 2 winnable draw three

StockDealCards = 1

-- C sets variables 'BAIZE', 'STOCK', FAN_*

function Build()

    if type(AddPile) ~= "function" then
        io.stderr:write("Build cannot find function AddPile\n")
        return
    end

    -- a stock pile is always created first, and filled with Packs of shuffled cards
    PileMoveTo(STOCK, 1, 1)
  
    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)
    
    local pile

    for x = 4, 7 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        SetPileAccept(pile, 1)
    end

    RESERVE = AddPile("Reserve", 1, 2, FAN_DOWN)
    for n = 1, 13 do
        local c = MoveCard(STOCK, RESERVE)
        SetCardProne(c, true)
    end
    SetCardProne(PilePeek(RESERVE), false)

    TABLEAUX = {}
    for x = 4, 7 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        MoveCard(STOCK, pile)
        TABLEAUX[#TABLEAUX+1] = pile
    end

end

function StartGame()
  STOCK_RECYCLES = 1
  SetPileRecycles(STOCK, STOCK_RECYCLES)
end

function CanTailBeMoved_Foundation(tail)
    return false, "You cannot move cards from a Foundation"
end

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

function CanTailBeMoved_Waste(tail)
    if TailLen(tail) > 1 then
        return false, "Only a single card can be moved from Waste"
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

function IsPileConformant_Foundation(pile)
    local c1 = PilePeek(pile)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(tail, n)

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
        local c2 = PileGet(tail, n)
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

function CardTapped(card)
  -- LogCard("CardTapped", card)
  if CardOwner(card) == STOCK then
    for i = 1, StockDealCards do
      MoveCard(STOCK, WASTE)
    end
  end
end

function PileTapped(pile)
  -- io.stdout:write("PileTapped\n")
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

function AfterMove()
    -- io.stdout:write("AfterMove\n")
    for i = 1, 4 do
        if PileLen(TABLEAUX[i]) == 0 then
            MoveCard(RESERVE, TABLEAUX[i])
        end
    end
end
