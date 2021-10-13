-- Quick Win

PACKS = 1
SUITS = 4
POWERMOVES = false
STOCK_RECYCLES = 1
-- CARD_FILTER = {1, 7, 8, 9, 10, 11, 12}  -- twos, threes, fours, fives, sixes, and kings removed
STRIP_CARDS = {12,13}

-- C sets variables 'BAIZE', 'STOCK', FAN_*

--[[
function LogCard(title, card)
  if not card then
    io.stderr:write(title .. " {nil}\n")
  elseif type(card) ~= "table" then
    io.stderr:write(title .. " {unknown}\n")
  else
    io.stderr:write(title .. " {ordinal:" .. card.ordinal .. " suit:" .. card.suit .. " color:" .. card.color .. " owner:" .. PileType(card.owner) .. "}\n")
  end
end

function LogTail(title, cards)
  for n=1, #cards do
    LogCard(title, cards[n])
  end
end
]]

function Build()

    if type(AddPile) ~= "function" then
        io.stderr:write("Build cannot find function AddPile\n")
        return
    end

    -- a stock pile is always created first, and filled with Packs of shuffled cards
    PileMoveTo(STOCK, 1, 1)

    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)

    FOUNDATIONS = {}
    for x = 7, 10 do
        local pile = AddPile("Foundation", x, 1, FAN_NONE)
        SetPileAccept(pile, 1)
        table.insert(FOUNDATIONS, pile)
    end

    MoveCard(STOCK, FOUNDATIONS[1], 1, 0)

    TABLEAUX = {}
    for x = 1, 10 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN)
        for n = 1, 2 do
          MoveCard(STOCK, pile)
        end
        PileDemoteCards(pile, 13)
        PilePromoteCards(pile, 1)
        table.insert(TABLEAUX, pile)
    end

    for x = 1, 10 do
        local pile = AddPile("Tableau", x, 4, FAN_DOWN)
        for n = 1, 2 do
          MoveCard(STOCK, pile)
        end
        PileDemoteCards(pile, 13)
        PilePromoteCards(pile, 1)
        table.insert(TABLEAUX, pile)
    end
end

function StartGame()
    STOCK_RECYCLES = 1
    SetPileRecycles(STOCK, STOCK_RECYCLES)
end

function Foundation_CanTailBeMoved(tail)
    return false, "You cannot move cards from a Foundation"
end

function Tableau_CanTailBeMoved(tail)
    local c1 = TailGet(tail, 1)
    for i = 2, TailLen(tail) do
        local c2 = TailGet(tail, i)
        if CardSuit(c1) ~= CardSuit(c2) then
            return false
        end
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false
        end
        c1 = c2
    end
    return true
end

function Waste_CanTailBeMoved(tail)
    if TailLen(tail) > 1 then
        return false, "Only a single card can be moved from Waste"
    end
    return true
end

function CanTailBeMoved(tail)
    local c1 = TailGet(tail, 1)
    local pile = CardOwner(c1)
    local fn = PileType(pile) .. "_CanTailBeMoved"
    -- io.stderr:write("type(" .. fn .. ") == " .. type(_G[fn]) .. "\n")

    if type(_G[fn]) == "function" then
        return _G[fn](tail)
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

            if CardSuit(c1) ~= CardSuit(c2) then
                return false, "Tableaux build in suit"
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
