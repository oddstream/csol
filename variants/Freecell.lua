-- Freecell

PACKS = 1
SUITS = 4
POWERMOVES = true

-- C sets variable 'BAIZE', 'STOCK', FAN_*

function Build()

  if type(AddPile) ~= "function" then
    io.stderr:write("Build cannot find function AddPile\n")
    return  
  end

    -- a stock pile is always created first, and filled with Packs of shuffled cards
  PileMoveTo(STOCK, 5, -5)  -- hide the stock off screen

  local pile

  for x = 1, 4 do
    pile = AddPile("Cell", x, 1, FAN_NONE)
  end

  for x = 5, 8 do
    pile = AddPile("Foundation", x, 1, FAN_NONE)
    SetPileAccept(pile, 1)
  end

  for x = 1, 4 do
    pile = AddPile("Tableau", x, 2, FAN_DOWN)
    for n = 1, 7 do
      MoveCard(STOCK, pile)
    end
    if EASY then
      PileDemoteCards(pile, 13)
      PilePromoteCards(pile, 1)
    end
  end

  for x = 5, 8 do
    pile = AddPile("Tableau", x, 2, FAN_DOWN)
    for n = 1, 6 do
      MoveCard(STOCK, pile)
    end
    if EASY then
      PileDemoteCards(pile, 13)
      PilePromoteCards(pile, 1)
    end
  end

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
        if POWERMOVES then
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
        else
            if TailLen(tail) > 1 then
                return false, "Can only move a single card"
            end
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
        else
            if PileLen(pile) == 0 then
                local c1 = TailGet(tail, 1)
                if CardOrdinal(c1) ~= 1 then
                    return false, "Foundation can only accept a 1, not a " .. math.floor(CardOrdinal(c1))
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
    elseif PileType(pile) == "Tableau" then
        if PileLen(pile) == 0 then
            -- do nothing, empty accept any card
        else
            local c1 = PilePeek(pile)
            for i = 1, TailLen(tail) do
                local c2 = TailGet(tail, i)
                if CardColor(c1) == CardColor(c2) then
                    return false, "Tableaux build in alternating colors"
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
                return false, "Tableaux build in alternating colors"
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
