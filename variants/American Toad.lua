-- American Toad

--[[
American Toad is a solitaire game using two decks of playing cards.
This game is similar to Canfield except that the tableau builds down in suit, and a partial tableau stack cannot be moved (only the top card or entire stack can be moved).
The object of the game is to move all cards to the foundations.
American Toad has eight tableau stacks.
Each tableau stack contains one card and builds down in suit wrapping from Ace to King, e.g. 3♠, 2♠, A♠, K♠...
There are also eight foundations that build up in suit, e.g. 7♦, 8♦, 9♦...
The game includes one reserve pile with twenty cards that can be played onto the tableau or foundations.
There is a deck usually at the bottom right that turns up one card at a time.
One card is dealt onto the first foundation. This rank will be used as a base for the other foundations.
The foundations build up in suit, wrapping from King to Ace as necessary.
]]

dofile("variants/~Library.lua")

function Wikipedia()
    return "https://en.wikipedia.org/wiki/American_Toad_(solitaire)"
end

function BuildPiles()

    AddPile("Stock", 1, 1, FAN_NONE, 2, 4)
    AddPile("Waste", 2, 1, FAN_RIGHT3)

    AddPile("Reserve", 4, 1, FAN_RIGHT)

    for x = 1, 8 do
        AddPile("Foundation", x, 2, FAN_NONE)
    end

    for x = 1, 8 do
        AddPile("Tableau", x, 3, FAN_DOWN, MOVE_ONE_OR_ALL)
    end

end

function StartGame()

    StockRecycles(1)

    local pile = Reserve.Pile
    for n = 1, 20 do
        local c = MoveCard(Stock.Pile, pile)
        CardProne(c, true)
    end
    CardProne(Last(pile), false)

    for _, pile in ipairs(Tableau.Piles) do
        MoveCard(Stock.Pile, pile)
    end

    MoveCard(Stock.Pile, Foundation.Piles[1])
    local c = First(Foundation.Piles[1])
    for _, pile in ipairs(Foundation.Piles) do
        PileLabel(pile, U[CardOrdinal(c)])
    end
end

-- TailMoveError constraints (Tableau only)

function Tableau.TailMoveError(tail)
    if Len(tail) == 1 then
        return nil
    end
    local c1 = First(tail)
    if Len(tail) ~= Len(CardOwner(c1)) then
        return "Can only move one card, or the whole pile"
    else
        for i = 2, Len(tail) do
            local c2 = Get(tail, i)
            local err = DownSuitWrap(c1, c2) if err then return err end
            c1 = c2
        end
    end
    return nil
end

-- TailAppendError constraints

function Foundation.TailAppendError(pile, tail)
    if Empty(pile) then
        local c1 = First(tail)
        if U[CardOrdinal(c1)] ~= PileLabel(pile) then
            return "An empty Foundation can only accept " .. PileLabel(pile) .. " not " .. U[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = UpSuitWrap(c1, c2) if err then return err end
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    if Empty(pile) then
        -- do nothing, empty accept any card
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = DownSuitWrap(c1, c2) if err then return err end
    end
    return nil
end

function Tableau.UnsortedPairs(pile)
    local unsorted = 0
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = DownSuitWrap(c1, c2)
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
    MoveCard(Stock.Pile, Waste.Pile)
end

function AfterMove()
	-- Empty spaces are filled automatically from the reserve.
    for _, pile in ipairs(Tableau.Piles) do
        if Empty(pile) then
            if Len(Reserve.Pile) > 0 then
                MoveCard(Reserve.Pile, pile)
            end
        end
    end

end
