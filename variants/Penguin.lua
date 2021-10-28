-- Penguin

-- https://www.parlettgames.uk/patience/penguin.html

dofile("variants/~Library.lua")

POWER_MOVES = false

SEED = 4072

function BuildPiles()

  if type(AddPile) ~= "function" then
    io.stderr:write("Build cannot find function AddPile\n")
    return
  end

  STOCK = AddPile("Stock", 5, -5, FAN_NONE, 1, 4)  -- hide the stock off screen

  local pile

  -- the flipper, seven cells
  CELLS = {}
  for x = 1, 7 do
    pile = AddPile("Cell", x, 1, FAN_NONE)
    table.insert(CELLS, pile)
  end

  FOUNDATIONS = {}
  for y = 1, 4 do
    pile = AddPile("Foundation", 8.5, y, FAN_NONE)
    table.insert(FOUNDATIONS, pile)
  end

  TABLEAUX = {}
  for x = 1, 7 do
    pile = AddPile("Tableau", x, 2, FAN_DOWN)
    table.insert(TABLEAUX, pile)
  end

  -- Shuffle a 52-card pack and deal the first card face up to the top left of the board.
  -- This card is called the Beak.
  local fnext = 1
  local faccept = CardOrdinal(Last(STOCK))
  for _, f in ipairs(FOUNDATIONS) do
    PileAccept(f, faccept)
  end
  MoveCard(STOCK, TABLEAUX[1])

  -- 49-card layout consisting of seven rows and seven columns
  for _, pile in ipairs(TABLEAUX) do
    while Len(pile) < 7 do
        if Empty(STOCK) then
            io.stderr:write("Oops - Stock is empty\n")
            break
        end
        -- As and when the other three cards of the same rank turn up in the deal,
        -- take them out and set them apart as foundations.
        if CardOrdinal(Last(STOCK)) == faccept then
            MoveCard(STOCK, FOUNDATIONS[fnext])
            fnext = fnext + 1
-- io.stderr:write("moved to foundation[" ..fnext .. "] " .. V[CardOrdinal(card)] .. "\n")
        else
            MoveCard(STOCK, pile)
-- io.stderr:write("moved to " .. PileType(pile) .. "[" .. x .. "] ordinal ".. V[CardOrdinal(card)] .. "\n")
        end
    end
  end

  if Len(STOCK) > 0 then
    io.stderr:write("Oops - there are still " .. Len(STOCK) .. " cards in the Stock\n")
  end

  -- When you empty a column, you may fill the space it leaves with a card one rank lower than the rank of the beak,
  -- together with any other cards attached to it in descending suit-sequence.
  -- For example, since the beak is a Ten, you can start a new column only with a Nine,
  -- or a suit-sequence headed by a Nine.

  local taccept = faccept - 1
  if taccept == 0 then
    taccept = 13
  end
  for _, tabpile in ipairs(TABLEAUX) do
    PileAccept(tabpile, taccept)
  end

end

-- CanTailBeMoved constraints

function Tableau.CanTailBeMoved(tail)
    local c1 = Get(tail, 1)
    for i = 2, Len(tail) do
        local c2 = Get(tail, i)
        local err = DownSuitWrap(c1, c2) if err then return false, err end
        c1 = c2
    end
    return true
end

-- CanTailBeAppended constraints

function Foundation.CanTailBeAppended(pile, tail)
    if Empty(pile) then
        local c1 = First(tail)
        if CardOrdinal(c1) ~= PileAccept(pile) then
            return false, "Foundation can only accept a " .. V[PileAccept(pile)] .. " not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = UpSuitWrap(c1, c2) if err then return false, err end
    end
    return true
end

function Tableau.CanTailBeAppended(pile, tail)
    if Empty(pile) then
        local c2 = First(tail)
        if CardOrdinal(c2) ~= PileAccept(pile) then
            return false, "Empty Tableau can accept a " .. V[PileAccept(pile)] .. " not a " .. V[CardOrdinal(c2)]
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = DownSuitWrap(c1, c2) if err then return false, err end
    end
    return true
end

-- IsPileConformant (Tableau only)

function Tableau.IsPileConformant(pile)
    local c1 = First(pile)
    for i = 2, Len(pile) do
        local c2 = Get(pile, n)
        local err = DownSuitWrap(c1, c2) if err then return false, err end
        c1 = c2
    end
    return true
end

-- SortedAndUnSorted (Tableau only)

function Tableau.SortedAndUnsorted(pile)
    local sorted = 0
    local unsorted = 0
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = DownSuitWrap(c1, c2)
        if err then
            unsorted = unsorted + 1
        else
            sorted = sorted + 1
        end
        c1 = c2
    end
    return sorted, unsorted
end
