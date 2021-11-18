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

STOCK_DEAL_CARDS = 1

function BuildPiles()

    AddPile("Stock", 1, 1, FAN_NONE, 2, 4)
    AddPile("Waste", 2, 1, FAN_RIGHT3)

    local pile

    pile = AddPile("Reserve", 4, 1, FAN_RIGHT)
    for n = 1, 20 do
        local c = MoveCard(Stock.Pile, pile)
        CardProne(c, true)
    end
    CardProne(Last(pile), false)
    
    for x = 1, 8 do
        pile = AddPile("Foundation", x, 2, FAN_NONE)
    end

    for x = 1, 8 do
        pile = AddPile("Tableau", x, 3, FAN_DOWN, MOVE_ONE_OR_ALL)
        MoveCard(Stock.Pile, pile)
    end

end

function StartGame()
    STOCK_RECYCLES = 1
    StockRecycles(STOCK_RECYCLES)

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

function Waste.TailAppendError(pile, tail)
    if CardOwner(First(tail)) ~= STOCK then
        return "The Waste can only accept cards from the Stock"
    end
    return nil
end

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

-- PileConformantError

function Tableau.PileConformantError(pile)
    local c1 = First(pile)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = DownSuitWrap(c1, c2) if err then return err end
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
