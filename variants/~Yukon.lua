-- Yukon

-- https://en.wikipedia.org/wiki/Yukon_(solitaire)

dofile("variants/~Library.lua")

function Wikipedia()
    return "https://en.wikipedia.org/wiki/Yukon_(solitaire)"
end

function BuildPiles()

    AddPile("Stock", -5, -5, FAN_NONE, 1, 4)
    
    local pile

    for y = 1, 4 do
        pile = AddPile("Foundation", 8.5, y, FAN_NONE)
        PileLabel(pile, U[1])
    end

    for x = 1, 7 do
        pile = AddPile("Tableau", x, 1, FAN_DOWN, MOVE_ANY)
        if not RELAXED then
            PileLabel(pile, U[13])
        end
    end
end

function StartGame()

    local card
    local pile = Tableau.Piles[1]
    MoveCard(Stock.Pile, pile)

    local dealDown = 1
    local dealUp = 5
    for x = 2, 7 do
        pile = Tableau.Piles[x]
        for c = 1, dealDown do
            card = MoveCard(Stock.Pile, pile)
            CardProne(card, true)
        end
        for c = 1, dealUp do
            card = MoveCard(Stock.Pile, pile)
            -- CardProne(card, false)
        end
        dealDown = dealDown + 1
    end

    if not Empty(Stock.Pile) then
        io.stderr:write("Oops! There are still " .. Len(Stock.Pile) .. " cards in the Stock\n")
    end
    if RELAXED then
        Toast("Relaxed version - any card may be placed in an empty pile")
    end
end

-- TailMoveError constraints (Tableau only)

function Tableau.TailMoveError(tail)
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
        local c2 = First(tail)
        local err = UpSuit(c1, c2) if err then return err end
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    if Empty(pile) then
        if not RELAXED then
            local c1 = Get(tail, 1)
            if CardOrdinal(c1) ~= 13 then
                return "Empty Tableaux can only accept a King, not a " .. V[CardOrdinal(c1)]
            end
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = DownAltColor(c1, c2) if err then return err end
    end
    return nil
end

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
