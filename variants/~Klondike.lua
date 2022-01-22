-- Klondike

dofile("variants/~Library.lua")

-- C sets variables 'BAIZE', FAN_*, MOVE_* and tables to hold pile functions and piles

function Wikipedia()
    return "https://en.wikipedia.org/wiki/Klondike_(solitaire)"
end

function BuildPiles()
    AddPile("Stock", 1, 1, FAN_NONE, 1, 4)
    AddPile("Waste", 2, 1, FAN_RIGHT3)

    for x = 4, 7 do
        local pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileLabel(pile, U[1])
    end

    for x = 1, 7 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN, MOVE_ANY)
        PileLabel(pile, U[13])
    end
end

function StartGame()
    StockRecycles(32767)

    local deal = 1
    for x = 1, 7 do
        local pile = Tableau.Piles[x]
        for n = 1, deal do
          local c = MoveCard(Stock.Pile, pile)
          CardProne(c, true)
        end
        CardProne(Last(pile), false)
        deal = deal + 1
    end

    MoveCard(Stock.Pile, Waste.Pile)
end

function AfterMove()
    if Empty(Waste.Pile) and not Empty(Stock.Pile) then
        MoveCard(Stock.Pile, Waste.Pile)
    end
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

-- UnsortedPairs

function Tableau.UnsortedPairs(pile)
    local unsorted = 0
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = DownAltColor(c1, c2)
        if err then
            unsorted = unsorted + 1
        end
        c1 = c2
    end
    return unsorted
end

-- Actions

function Stock.PileTapped(pile)
    recycles = StockRecycles()
    if recycles == 0 then
        return "No more Stock recycles"
    elseif 1 == recycles then
        Toast("Last Stock recycle")
    elseif 2 == recycles then
        Toast("One Stock recycle remaining")
    end
    if Len(Waste.Pile) > 0 then
        while Len(Waste.Pile) > 0 do
            MoveCard(Waste.Pile, Stock.Pile)
        end
        recycles = recycles - 1
        StockRecycles(recycles)
    end
end

function Stock.TailTapped(tail)
    for i = 1, STOCK_DEAL_CARDS do
        MoveCard(Stock.Pile, Waste.Pile)
    end
end
