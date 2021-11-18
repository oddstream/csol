-- Klondike

dofile("variants/~Library.lua")

-- C sets variables 'BAIZE', FAN_*, and tables to hold pile functions and piles

STOCK_RECYCLES = 32767

function BuildPiles()

    AddPile("Stock", 1, 1, FAN_NONE, 1, 4)
    AddPile("Waste", 2, 1, FAN_RIGHT3)
    
    local pile

    for x = 4, 7 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileLabel(pile, U[1])
    end

    local deal = 1
    for x = 1, 7 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN, MOVE_ANY)
        PileLabel(pile, U[13])

        for n = 1, deal do
          local c = MoveCard(Stock.Pile, pile)
          CardProne(c, true)
        end
        CardProne(Last(pile), false)
        deal = deal + 1
    end

end

function StartGame()
    STOCK_RECYCLES = 32767
    StockRecycles(STOCK_RECYCLES)
end

-- TailMoveError constraints (Tableau only)

function Tableau.TailMoveError(tail)
    local c1 = Get(tail, 1)
    for i = 2, Len(tail) do
        local c2 = Get(tail, i)
        local err = DownAltColor(c1, c2)  if err then return err end
        c1 = c2
    end
    return nil
end

-- TailAppendError constraints

function Waste.TailAppendError(pile, tail)
    if Len(tail) > 1 then
        return "The Waste can only accept a single card"
    end
    if CardOwner(Get(tail, 1)) ~= Stock.Pile then
        return "The Waste can only accept cards from the Stock"
    end
    return nil
end

function Foundation.TailAppendError(pile, tail)
    if Len(pile) == 0 then
        local c1 = Get(tail, 1)
        if CardOrdinal(c1) ~= 1 then
            return "Foundation can only accept an Ace, not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = Get(tail, 1)
        local err = UpSuit(c1, c2)  if err then return err end
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    if Len(pile) == 0 then
        local c1 = Get(tail, 1)
        if CardOrdinal(c1) ~= 13 then
            return "Empty Tableaux can only accept a King, not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = Get(tail, 1)
        local err = DownAltColor(c1, c2)  if err then return err end
    end
    return nil
end

-- PileConformantError

function Tableau.PileConformantError(pile)
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = DownAltColor(c1, c2)  if err then return err end
        c1 = c2
    end
    return nil
end

-- SortedAndUnSorted (_Tableau only)

function Tableau.SortedAndUnsorted(pile)
    local sorted = 0
    local unsorted = 0
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = DownAltColor(c1, c2)
        if not err then
            sorted = sorted + 1
        else
            unsorted = unsorted + 1
        end
        c1 = c2
    end
    return sorted, unsorted
end

-- Actions

function Stock.PileTapped(pile)
    if STOCK_RECYCLES == 0 then
        return "No more Stock recycles"
    elseif 1 == STOCK_RECYCLES then
        Toast("Last Stock recycle")
    elseif 2 == STOCK_RECYCLES then
        Toast("One Stock recycle remaining")
    end
    if Len(Waste.Pile) > 0 then
        while Len(Waste.Pile) > 0 do
            MoveCard(Waste.Pile, Stock.Pile)
        end
        STOCK_RECYCLES = STOCK_RECYCLES - 1
        StockRecycles(STOCK_RECYCLES)
    end
end

function Stock.TailTapped(tail)
    for i = 1, STOCK_DEAL_CARDS do
        MoveCard(Stock.Pile, Waste.Pile)
    end
end
