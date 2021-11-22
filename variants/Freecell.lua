-- Freecell

dofile("variants/~Library.lua")

local function DemoteCards(pile, ord)
    -- yes, I know, a bubble sort
    local hasChanged
    local itemCount = Len(pile)
    repeat
        hasChanged = false
        itemCount = itemCount - 1
        for i = 1, itemCount do
            local c1 = Get(pile, i)
            local c2 = Get(pile, i + 1)
            if CardOrdinal(c1) ~= ord and CardOrdinal(c2) == ord then
                SwapCards(c1, c2)
                hasChanged = true
            end
        end
    until not hasChanged 
end

local function PromoteCards(pile, ord)
    local hasChanged
    local itemCount = Len(pile)
    repeat
        hasChanged = false
        itemCount = itemCount - 1
        for i = 1, itemCount do
            local c1 = Get(pile, i)
            local c2 = Get(pile, i + 1)
            if CardOrdinal(c1) == ord and CardOrdinal(c2) ~= ord then
                SwapCards(c1, c2)
                hasChanged = true
            end
        end
    until not hasChanged 
end

function Wikipedia()
    return "https://en.wikipedia.org/wiki/FreeCell"
end

function BuildPiles()

  if type(AddPile) ~= "function" then
    io.stderr:write("Build cannot find function AddPile\n")
    return
  end

  AddPile("Stock", 5, -5, FAN_NONE, 1, 4)  -- hide the stock off screen

  for x = 1, 4 do
    AddPile("Cell", x, 1, FAN_NONE)
  end

  for x = 5, 8 do
    local pile = AddPile("Foundation", x, 1, FAN_NONE)
    PileLabel(pile, U[1])
  end

  for x = 1, 4 do
    AddPile("Tableau", x, 2, FAN_DOWN, MOVE_ONE_PLUS)
  end
  for x = 5, 8 do
    AddPile("Tableau", x, 2, FAN_DOWN, MOVE_ONE_PLUS)
  end

end

function StartGame()
    for x = 1, 4 do
        pile = Tableau.Piles[x]
        for n = 1, 7 do
          MoveCard(Stock.Pile, pile)
        end
        if EASY then
            DemoteCards(pile, 13)
            PromoteCards(pile, 1)
        end
      end
      for x = 5, 8 do
        pile = Tableau.Piles[x]
        for n = 1, 6 do
          MoveCard(Stock.Pile, pile)
        end
        if EASY then
            DemoteCards(pile, 13)
            PromoteCards(pile, 1)
        end
    end
    if EASY then
        Toast("Kings have been buried and Aces moved to the front")
    end
end

-- TailMoveError constraints

function Tableau.TailMoveError(tail)
    if Len(tail) > 1 then
        local c1 = Get(tail, 1)
        for i = 2, Len(tail) do
            local c2 = Get(tail, i)
            local err = DownAltColor(c1, c2) if err then return err end
            c1 = c2
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
        local c2 = Get(pile, i)
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