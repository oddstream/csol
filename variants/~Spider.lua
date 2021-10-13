-- Spider

POWERMOVES = false

-- C sets variables 'BAIZE', 'STOCK', FAN_*

function Build()

    if type(AddPile) ~= "function" then
        io.stderr:write("Build cannot find function AddPile\n")
        return
    end

    -- a stock pile is always created first, and filled with Packs of shuffled cards
    PileMoveTo(STOCK, 1, 1)
    SetPileRecycles(STOCK, 0)

    TABLEAU = {}
  
    for x = 3, 10 do
        local pile = AddPile("Discard", x, 1, FAN_NONE)
    end

    for x = 1, 4 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN)
        for n=1,4 do
          local c = MoveCard(STOCK, pile)
          SetCardProne(c, true)
        end
        MoveCard(STOCK, pile)
        TABLEAU[x] = pile
    end

    for x = 5, 10 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN)
        for n=1,3 do
          local c = MoveCard(STOCK, pile)
          SetCardProne(c, true)
        end
        MoveCard(STOCK, pile)
        TABLEAU[x] = pile
    end
end

function CanTailBeMoved(tail)
    if TailLen(tail) == 0 then
        return false, "Empty tail"
    end
    local c1 = TailGet(tail, 1)
    local pile = CardOwner(c1)
    if PileType(pile) == "Discard" then
        return false, "You cannot move cards from a Discard"
    elseif PileType(pile) == "Tableau" then
        for i = 2, TailLen(tail) do
            local c2 = TailGet(tail, i)

            if CardSuit(c1) ~= CardSuit(c2) then
                return false, "Moved cards must all have the same suit"
            end
            if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
                return false, "Moved cards must be in descending order"
            end

            c1 = c2
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
    if PileType(pile) == "Discard" then
        if TailLen(tail) ~= 13 then
            return false, "Discard can only accept 13 cards"
        else
            local c1 = TailGet(tail, 0)
            if CardOrdinal(c1) ~= 13 then
                return false, "Can only discard from a 13"
            end
            for i = 2, TailLen(tail) do
                local c2 = TailGet(tail, i)

                if CardSuit(c1) ~= CardSuit(c2) then
                    return false, "Discarded piles must be all the same suit"
                end
                if CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
                    return false, "Discarded piles build up"
                end

                c1 = c2
            end
        end
    elseif PileType(pile) == "Tableau" then
        if PileLen(pile) == 0 then
            -- accept any card
        else
            local c1 = PilePeek(pile)
            local c2 = TailGet(tail, 1)
            if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
                return false, "Tableaux build down"
            end
        end
    else
        io.stderr:write("CanTailBeAppended: unknown pile type " .. PileType(pile) .. "\n")
    end
    return true
end

function IsPileConformant(pile)
    if PileType(pile) == "Discard" or PileType(pile) == "Tableau" then
        local c1 = PilePeek(pile)
        for i = 2, PileLen(pile) do
            local c2 = PileGet(tail, n)

            if CardSuit(c1) ~= CardSuit(c2) then
                return false
            end
            if CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
                return false
            end

            c1 = c2
        end
    else
        io.stderr:write("IsPileConformant: unknown pile type " .. PileType(pile) .. "\n")
    end
    return true
end

function CardTapped(card)
    -- LogCard("CardTapped", card)
    local errMsg = nil
    local tabCards = 0
    local emptyTabs = 0
    if CardOwner(card) == STOCK then
      for _, tab in ipairs(TABLEAU) do
        if PileLen(tab) == 0 then
          emptyTabs = emptyTabs + 1
        else
          tabCards = tabCards + PileLen(tab)
        end
      end
      if emptyTabs > 0 and tabCards >= #TABLEAU then
        errMsg = "All empty tableaux must be filled before dealing a new row"
      else
        for _, tab in ipairs(TABLEAU) do
          MoveCard(STOCK, tab)
        end
      end
    end

    return errMsg
end
