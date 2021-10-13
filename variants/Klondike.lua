-- Klondike

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

    local deal = 1
    for x = 1, 7 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
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

function CanTailBeMoved(tail)
    if TailLen(tail) == 0 then
        return false, "Empty tail"
    end
    local c1 = TailGet(tail, 1)
    local pile = CardOwner(c1)
    if PileType(pile) == "Foundation" then
        return false, "You cannot move cards from a Foundation"
    elseif PileType(pile) == "Tableau" then
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
    elseif PileType(pile) == "Waste" then
        if TailLen(tail) > 1 then
            return false, "Only a single card can be moved from Waste"
        end
    else
        io.stderr:write("CanTailBeMoved: unknown pile type " .. PileType(pile) .. "\n")
    end
    return true
end

function CanTailBeAppended(pile, tail)
    if TailLen(tail) == 0 then
        return false, "Empty tail"
    end
    if PileType(pile) == "Foundation" then
        if TailLen(tail) > 1 then
            return false, "Foundation can only accept a single card"
        elseif PileLen(pile) == 0 then
            local c1 = TailGet(tail, 1)
            if CardOrdinal(c1) ~= 1 then
                return false, "Foundation can only accept a 1, not a " .. math.floor(CardOrdinal(c1))
            end
        else
            local c1 = PilePeek(pile)
            for i = 1, TailLen(tail) do
                local c2 = TailGet(tail, i)

                if CardSuit(c1) ~= CardSuit(c2) then
                    return false, "Foundations must be built in suit"
                end
                if CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
                    return false, "Foundations build up"
                end

                c1 = c2
            end
        end
    elseif PileType(pile) == "Tableau" then
        if PileLen(pile) == 0 then
            local c1 = TailGet(tail, 1)
            if CardOrdinal(c1) ~= 13 then
                return false, "Empty Tableaux can only accept a 13, not a " .. math.floor(CardOrdinal(c1))
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
    else
        io.stderr:write("CanTailBeAppended: unknown pile type " .. PileType(pile) .. "\n")
    end
    return true
end

function IsPileConformant(pile)
    if PileType(pile) == "Foundation" then
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
    elseif PileType(pile) == "Tableau" then
        local c1 = PilePeek(pile)
        for i = 2, PileLen(pile) do
            local c2 = PileGet(tail, n)

            if CardColor(c1) == CardColor(c2) then
                return false, "Tableaux build in alternate color"
            end
            if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
                return false, "Tableaux build down"
            end

            c1 = c2
        end
    else
        io.stderr:write("IsPileConformant: unknown pile type " .. PileType(pile) .. "\n")
    end
    return true
end

function CardTapped(card)
    if CardOwner(card) == STOCK then
        for i=1,StockDealCards do
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
          return nil
        end
    elseif pile == WASTE then
        if PileLen(STOCK) > 0 then
            MoveCard(STOCK, WASTE)
            return nil
        end
    end
end
