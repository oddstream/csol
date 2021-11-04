-- Freecell

dofile("variants/~Library.lua")

POWER_MOVES = true
EASY_SEEDS = {39675,50060,50419,10982}

function BuildPiles()

  if type(AddPile) ~= "function" then
    io.stderr:write("Build cannot find function AddPile\n")
    return
  end

  AddPile("Stock", 5, -5, FAN_NONE, 1, 4)  -- hide the stock off screen

  local pile

  for x = 1, 4 do
    pile = AddPile("Cell", x, 1, FAN_NONE)
  end

  for x = 5, 8 do
    pile = AddPile("Foundation", x, 1, FAN_NONE)
    PileAccept(pile, 1)
  end

  for x = 1, 4 do
    pile = AddPile("Tableau", x, 2, FAN_DOWN)
    for n = 1, 7 do
      MoveCard(Stock.Pile, pile)
    end
    if EASY then
      PileDemoteCards(pile, 13)
      PilePromoteCards(pile, 1)
    end
  end
  for x = 5, 8 do
    pile = AddPile("Tableau", x, 2, FAN_DOWN)
    for n = 1, 6 do
      MoveCard(Stock.Pile, pile)
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

-- TailMoveError constraints

function Tableau.TailMoveError(tail)
    if POWER_MOVES then
        local c1 = Get(tail, 1)
        for i = 2, Len(tail) do
            local c2 = Get(tail, i)
            local err = DownAltColor(c1, c2) if err then return err end
            c1 = c2
        end
    else
        if Len(tail) > 1 then
            return "Can only move a single card"
        end
    end
    return nil
end

-- TailAppendError constraints

function Foundation.TailAppendError(pile, tail)
    if Empty(pile) then
        local c1 = First(tail)
        if CardOrdinal(c1) ~= 1 then
            return "Foundations can only accept an Ace, not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = UpSuit(c1, c2) if err then return err end
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    if Empty(pile) then
        -- do nothing, empty accept any card
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = DownAltColor(c1, c2) if err then return err end
    end
    return nil
end

-- PileConformantError (Tableau only)

function Tableau.PileConformantError(pile)
    local c1 = First(pile)
    for i = 2, Len(pile) do
        local c2 = Get(pile, n)
        local err = DownAltColor(c1, c2) if err then return err end
        c1 = c2
    end
    return nil
end

-- SortedAndUnSorted (Tableau only)

function Tableau.SortedAndUnsorted(pile)
    local sorted = 0
    local unsorted = 0
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = DownAltColor(c1, c2)
        if err then
            unsorted = unsorted + 1
        else
            sorted = sorted + 1
        end
        c1 = c2
    end
    return sorted, unsorted
end