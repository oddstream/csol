-- Freecell

V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}
POWER_MOVES = true
EASY_SEEDS = {39675,50060,50419}

function BuildPiles()

  if type(AddPile) ~= "function" then
    io.stderr:write("Build cannot find function AddPile\n")
    return
  end

  STOCK = AddPile("Stock", 5, -5, FAN_NONE, 1, 4)  -- hide the stock off screen

  local pile

  CELLS = {}
  for x = 1, 4 do
    pile = AddPile("Cell", x, 1, FAN_NONE)
    table.insert(CELLS, pile)
  end

  FOUNDATIONS = {}
  for x = 5, 8 do
    pile = AddPile("Foundation", x, 1, FAN_NONE)
    PileAccept(pile, 1)
    table.insert(FOUNDATIONS, pile)
  end

  TABLEAUX = {}
  for x = 1, 4 do
    pile = AddPile("Tableau", x, 2, FAN_DOWN)
    table.insert(TABLEAUX, pile)
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
    table.insert(TABLEAUX, pile)
    for n = 1, 6 do
      MoveCard(STOCK, pile)
    end
    if EASY then
      PileDemoteCards(pile, 13)
      PilePromoteCards(pile, 1)
    end
  end

end

function StartGame()
    if EASY then
        Toast("Kings have been buried and Aces moved to the front")
    end
end

-- CanTailBeMoved constraints

function Tableau.CanTailBeMoved(tail)
    if POWER_MOVES then
        local c1 = TailGet(tail, 1)
        for i = 2, TailLen(tail) do
            local c2 = TailGet(tail, i)
            if CardColor(c1) == CardColor(c2) then
                return false, "Moved cards must be in alternating colors"
            end
            if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
                return false, "Moved cards must be in descending rank"
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

function Foundation.CanTailBeAppended(pile, tail)
    if PileLen(pile) == 0 then
        local c1 = TailGet(tail, 1)
        if CardOrdinal(c1) ~= 1 then
            return false, "Foundations can only accept an Ace, not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = PilePeek(pile)
        local c2 = TailGet(tail, 1)
        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Foundations must be built in suit"
        end
        if CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
            return false, "Foundations build up in rank"
        end
    end
    return true
end

function Tableau.CanTailBeAppended(pile, tail)
    if PileLen(pile) == 0 then
        -- do nothing, empty accept any card
    else
        local c1 = PilePeek(pile)
        local c2 = TailGet(tail, 1)
        if CardColor(c1) == CardColor(c2) then
            return false, "Tableaux build in alternating colors"
        end
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false, "Tableaux build down in rank"
        end
    end
    return true
end

-- IsPileConformant (Tableau only)

function Tableau.IsPileConformant(pile)
    local c1 = PilePeek(pile)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, n)
        if CardColor(c1) == CardColor(c2) then
            return false, "Tableaux build in alternating colors"
        end
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return false, "Tableaux build down in rank"
        end
        c1 = c2
    end
    return true
end

-- SortedAndUnSorted (Tableau only)

function Tableau.SortedAndUnsorted(pile)
    local sorted = 0
    local unsorted = 0
    local c1 = PileGet(pile, 1)
    for i = 2, PileLen(pile) do
        local c2 = PileGet(pile, i)
        if CardColor(c1) ~= CardColor(c2) then
            unsorted = unsorted + 1
        elseif CardOrdinal(c1) == CardOrdinal(c2) + 1 then
            sorted = sorted + 1
        else
            unsorted = unsorted + 1
        end
        c1 = c2
    end
    return sorted, unsorted
end
