-- Simple Simon

dofile("variants/~Library.lua")

function Wikipedia()
    return "https://en.wikipedia.org/wiki/Simple_Simon_(solitaire)"
end

function BuildPiles()

    AddPile("Stock", -5, -5, FAN_NONE, 1, 4)    -- hidden off screen

    for x = 4, 7 do
        AddPile("Discard", x, 1, FAN_NONE)
    end

    for x = 1, 10 do
        AddPile("Tableau", x, 2, FAN_DOWN, MOVE_ANY)
    end

end

function StartGame()

    for x = 1, 3 do
        local pile = Tableau.Piles[x]
        for n = 1, 8 do
            MoveCard(Stock.Pile, pile)
        end
    end
    local deal = 7
    for x = 4, 10 do
        local pile = Tableau.Piles[x]
        for n = 1, deal do
          local c = MoveCard(Stock.Pile, pile)
        end
        deal = deal - 1
    end

    if Len(Stock.Pile) ~= 0 then
        io.stdout:write("Oops, there are " .. Len(Stock.Pile) .. " cards still in the Stock\n")
    end
end

-- TailMoveError constraints (Tableau only)

function Tableau.TailMoveError(tail)
    -- A sequence of cards, decrementing in rank and of the same suit, can be moved as one
    local c1 = Get(tail, 1)
    for i = 2, Len(tail) do
        local c2 = Get(tail, i)
        local err = DownSuit(c1, c2) if err then return err end
        c1 = c2
    end
    return nil
end

-- TailAppendError constraints

function Discard.TailAppendError(pile, tail)
    -- C will have checked that there are (13 - number of cards in a suit) cards in the tail

    local c1 = Get(tail, 1)
    if CardOrdinal(c1) ~= 13 then
        return "Can only discard starting from a King, not a " .. V[CardOrdinal(c1)]
    end
    for i = 2, Len(tail) do
        local c2 = Get(tail, i)
        local err = DownSuit(c1, c2) if err then return err end
        c1 = c2
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    -- A card can be placed on any card on the top of a column whose rank is greater than it by one (with no cards that can be placed above an Ace).
    if Empty(pile) then
        -- An empty column may be filled by any card
    else
        local c1 = Last(pile)
        local c2 = First(tail, 1)
            -- K Q J 10 9 .. 2 1
        if CardOrdinal(c1) - 1 ~= CardOrdinal(c2) then
            return "Tableaux build down in rank"
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

-- Actions
