-- Duchess

--[[
    https://en.wikipedia.org/wiki/Duchess_(solitaire)
    Duchess (also Dutchess) is a patience or solitaire card game which uses a deck of 52 playing cards.
    It has all four typical features of a traditional solitaire game: a tableau, a reserve, a stock and a waste pile, and is quite easy to win.
    It is closely related to Canfield.
]]

V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}
POWERMOVES = false

StockDealCards = 1

function BuildPiles()

    STOCK = AddPile("Stock", 2, 2, FAN_NONE, 1, 4)
  
    local pile

    RESERVES = {}
    for i = 1, 4 do
        pile = AddPile("Reserve", (i*2), 1, FAN_RIGHT3)
        table.insert(RESERVES, pile)
        for j = 1, 3 do
            MoveCard(STOCK, pile)
        end
    end

    WASTE = AddPile("Waste", 2, 3, FAN_DOWN3)
    
    FOUNDATIONS = {}
    for x = 4, 7 do
        pile = AddPile("Foundation", x, 2, FAN_NONE)
        table.insert(FOUNDATIONS, pile)
    end

    TABLEAUX = {}
    for x = 4, 7 do
        pile = AddPile("Tableau", x, 3, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        MoveCard(STOCK, pile)
    end

end

function StartGame()
    STOCK_RECYCLES = 1
    SetPileRecycles(STOCK, STOCK_RECYCLES)
    FOUNDATION_ACCEPT = 0
    for _, v in pairs(FOUNDATIONS) do
        SetPileAccept(v, 0)
    end
end

-- CanTailBeMoved constraints

function CanTailBeMoved_Waste(tail)
    if TailLen(tail) > 1 then
        return false, "Only a single card can be moved from the Waste"
    end
    return true
end

function CanTailBeMoved_Foundation(tail)
    return false, "You cannot move cards from a Foundation"
end

function CanTailBeMoved_Tableau(tail)
    local c1 = TailGet(tail, 1)
    for i = 2, TailLen(tail) do
        local c2 = TailGet(tail, i)
        if CardColor(c1) == CardColor(c2) then
            return false, "Card must be in alternating colors"
        end
        if CardOrdinal(c1) == 1 and CardOrdinal(c2) == 13 then
            -- Ranking is continuous in the tableau as Kings can be placed over Aces.
        elseif CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false, "Cards must be in increasing value"
        end
        c1 = c2
    end
    return true
end

function CanTailBeMoved_Waste(tail)
    if TailLen(tail) > 1 then
        return false, "Only a single card can be moved from Waste"
    end
    return true
end

function CanTailBeMoved_Reserve(tail)
    if TailLen(tail) > 1 then
        return false, "Only a single card can be moved from a Reserve"
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
        return false, "Foundations can only accept a single card"
    elseif PileLen(pile) == 0 then
        local c1 = TailGet(tail, 1)
        if FOUNDATION_ACCEPT == 0 then
            -- To start the game, the player will choose among the top cards of the reserve fans which will start the first foundation pile.
            -- Once he/she makes that decision and picks a card, the three other cards with the same rank, whenever they become available, will start the other three foundations.
            if PileType(CardOwner(c1)) ~= "Reserve" then
                return false, "The first Foundation card must come from a Reserve"
            end
            FOUNDATION_ACCEPT = CardOrdinal(c1)
            for _, pile in ipairs(FOUNDATIONS) do
                SetPileAccept(pile, FOUNDATION_ACCEPT)
            end
        end
        if CardOrdinal(c1) ~= FOUNDATION_ACCEPT then
            return false, "Foundation can only accept a " .. V[FOUNDATION_ACCEPT] .. ", not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = PilePeek(pile)
        local c2 = TailGet(tail, 1)
        -- The foundations are built up by suit and ranking is continuous as Aces are placed over Kings. 
        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Foundations must be built in suit"
        end
        if CardOrdinal(c1) == 13 and CardOrdinal(c2) == 1 then
            -- 
        elseif CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
            return false, "Foundations build up"
        end
    end
    return true
end

function CanTailBeAppended_Tableau(pile, tail)
    local c1 = TailGet(tail, 1)
    if PileLen(pile) == 0 then
        -- Spaces that occur on the tableau are filled with any top card in the reserve.
        -- If the entire reserve is exhausted however, it is not replenished; spaces that occur after this point have to be filled with cards from the waste pile or, if a wastepile has not been made yet, the stock.
        if PileType(CardOwner(c1)) == "Waste" then
            for _, res in ipairs(RESERVES) do
                if PileLen(res) > 0 then
                    return false, "An empty Tableau must be filled from a Reserve"
                end
            end
        end
    else
        local c1 = PilePeek(pile)
        for i = 1, TailLen(tail) do
            local c2 = TailGet(tail, i)
            if CardColor(c1) == CardColor(c2) then
                return false, "Tableaux build in alternate color"
            end
            if CardOrdinal(c1) == 1 and CardOrdinal(c2) == 13 then
                --
            elseif CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
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
        local c2 = PileGet(tail, n)
        if CardColor(c1) == CardColor(c2) then
            return false, "Tableaux build in alternate color"
        end
        if CardOrdinal(c1) == 1 and CardOrdinal(c2) == 13 then
            -- wrap from Ace to King
        elseif CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
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

function CardTapped(card)
  if CardOwner(card) == STOCK then
    for i = 1, StockDealCards do
      MoveCard(STOCK, WASTE)
    end
  end
end

function PileTapped(pile)
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
    -- for i = 1, 4 do
    --     if PileLen(TABLEAUX[i]) == 0 then
    --         MoveCard(RESERVE, TABLEAUX[i])
    --     end
    -- end
end
