-- Spider

dofile("variants/~Library.lua")

function Wikipedia()
    return "https://en.wikipedia.org/wiki/Spider_(solitaire)"
end

function BuildPiles()

    AddPile("Stock", 1, 1, FAN_NONE, PACKS, SUITS)

    for x = 3, 10 do
        AddPile("Discard", x, 1, FAN_NONE)
    end

    for x = 1, 10 do
        AddPile("Tableau", x, 2, FAN_DOWN, MOVE_ANY)
    end
end

function StartGame()

    StockRecycles(0)

    for x = 1, 4 do
        local pile = Tableau.Piles[x]
        for n= 1, 4 do
          local c = MoveCard(Stock.Pile, pile)
          CardProne(c, true)
        end
        MoveCard(Stock.Pile, pile)
    end
    for x = 5, 10 do
        local pile = Tableau.Piles[x]
        for n= 1, 3 do
          local c = MoveCard(Stock.Pile, pile)
          CardProne(c, true)
        end
        MoveCard(Stock.Pile, pile)
    end
end

function Tableau.TailMoveError(tail)
    local c1 = Get(tail, 1)
    for i = 2, Len(tail) do
        local c2 = Get(tail, i)
        local err = DownSuit(c1, c2) if err then return err end
        c1 = c2
    end
    return nil
end

function Discard.TailAppendError(pile, tail)
    if Len(tail) ~= 13 then
        return "Discard can only accept 13 cards"
    else
        local c1 = First(tail)
        if CardOrdinal(c1) ~= 13 then
            return "Can only discard from a 13, not a " .. V[CardOrdinal(c1)]
        end
        for i = 2, Len(tail) do
            local c2 = Get(tail, i)
            local err = DownSuit(c1, c2) if err then return err end
            c1 = c2
        end
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    if Len(pile) == 0 then
        -- accept any card
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
            return "Tableaux build down"
        end
    end
    return nil
end

function Tableau.UnsortedPairs(pile)
    local unsorted = 0
    for _, c in ipairs(CardPairs(pile)) do
        local err = DownSuit(c[1], c[2])
        if err then
            unsorted = unsorted + 1
        end
    end
    return unsorted
end

function Stock.PileTapped(pile)
    Toast("No more cards in Stock")
end

function Stock.TailTapped(tail)

    local errMsg = nil
    local tabCards = 0
    local emptyTabs = 0

    for _, tab in ipairs(Tableau.Piles) do
        if Len(tab) == 0 then
            emptyTabs = emptyTabs + 1
        else
            tabCards = tabCards + Len(tab)
        end
    end
    if emptyTabs > 0 and tabCards >= #Tableau.Piles then
        Toast("All empty tableaux must be filled before dealing a new row")
    else
        for _, tab in ipairs(Tableau.Piles) do
            MoveCard(Stock.Pile, tab)
        end
    end
end
