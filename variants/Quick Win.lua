-- Quick Win

V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}
PACKS = 1
SUITS = 4
POWERMOVES = false
-- CARD_FILTER = {1, 7, 8, 9, 10, 11, 12}  -- twos, threes, fours, fives, sixes, and kings removed
STRIP_CARDS = {12,13}

-- C sets variables 'BAIZE', 'STOCK', FAN_*

function Build()

    if type(AddPile) ~= "function" then
        io.stderr:write("Build cannot find function AddPile\n")
        return
    end

    -- a stock pile is always created first, and filled with Packs of shuffled cards
    PileMoveTo(STOCK, 1, 1)

    local pile

    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)

    FOUNDATIONS = {}
    for x = 7, 10 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        table.insert(FOUNDATIONS, pile)
        SetPileAccept(pile, 1)
    end

    MoveCard(STOCK, FOUNDATIONS[1], 1, 0)

    TABLEAUX = {}
    for x = 1, 10 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for n = 1, 2 do
          MoveCard(STOCK, pile)
        end
        PileDemoteCards(pile, 13)
        PilePromoteCards(pile, 1)
    end
    for x = 1, 10 do
        pile = AddPile("Tableau", x, 4, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        for n = 1, 2 do
          MoveCard(STOCK, pile)
        end
        PileDemoteCards(pile, 13)
        PilePromoteCards(pile, 1)
    end
end

function StartGame()
    STOCK_RECYCLES = 3
    SetPileRecycles(STOCK, STOCK_RECYCLES)
end

function CanTailBeMoved_Foundation(tail)
    return false, "You cannot move cards from a Foundation"
end

function CanTailBeMoved_Tableau(tail)
    local c1 = TailGet(tail, 1)
    for i = 2, TailLen(tail) do
        local c2 = TailGet(tail, i)
        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Moved cards must be the same suit"
        end
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false, "Moved cards must be in descending order"
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

--[[
function CanTailBeMoved(tail)
    local c1 = TailGet(tail, 1)
    local pile = CardOwner(c1)
    local fn = "CanTailBeMoved_" .. PileType(pile) 
    -- io.stderr:write("type(" .. fn .. ") == " .. type(_G[fn]) .. "\n")
    if type(_G[fn]) == "function" then
        return _G[fn](pile, tail)
    else
        io.stderr:write(fn .. " is not a function\n")
    end
    return true
end
]]

function CanTailBeAppended_Foundation(pile, tail)
    if TailLen(tail) > 1 then
        return false, "Foundation can only accept a single card"
    elseif PileLen(pile) == 0 then
        local c1 = TailGet(tail, 1)
        if CardOrdinal(c1) ~= 1 then
            return false, "Foundation can only accept an Ace, not a " .. V[CardOrdinal(c1)]
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
            if not c2 then
                io.stderr:write("CanTailBeAppended: nil tail card at index " .. i .. "\n")
                break
            end
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
    local c1 = PileGet(pile, 1)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, i)
        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Foundations build in suit"
        end
        if CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
            return false, "Foundations build up"
        end
        c1 = c2
    end
    return true
end

function IsPileConformant_Tableau(pile)
    local c1 = PileGet(pile, 1)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, i)
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

function CardTapped(card)
    if CardOwner(card) == STOCK then
        MoveCard(STOCK, WASTE)
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
      if 0 == STOCK_RECYCLES then
        Toast("No Stock recycles remaining")
      elseif 1 == STOCK_RECYCLES then
        Toast("One Stock recycle remaining")
      else
        Toast(STOCK_RECYCLES .. " Stock recycles remaining")
      end
    end
  elseif pile == WASTE then
    if PileLen(STOCK) > 0 then
      MoveCard(STOCK, WASTE)
    end
  end
end
  
function AfterMove()
  -- io.stdout:write("AfterMove\n")
  for _, pile in ipairs(TABLEAUX) do
    if PileLen(pile) == 0 then
      if PileLen(WASTE) > 0 then
        MoveCard(WASTE, pile)
      elseif PileLen(STOCK) > 0 then
        MoveCard(STOCK, pile)
      end
    end
  end
  if PileLen(WASTE) == 0 then
    MoveCard(STOCK, WASTE)
  end
end
