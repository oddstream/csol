-- Duchess

--[[
    Duchess (also Dutchess) is a patience or solitaire card game which uses a deck of 52 playing cards.
    It has all four typical features of a traditional solitaire game: a tableau, a reserve, a stock and a waste pile, and is quite easy to win.
    It is closely related to Canfield.
]]

dofile("variants/~Library.lua")

STOCK_DEAL_CARDS = 1

function Wikipedia()
    return "https://en.wikipedia.org/wiki/Duchess_(solitaire)"
end

function BuildPiles()

    AddPile("Stock", 2, 2, FAN_NONE, 1, 4)
  
    local pile

    for i = 1, 4 do
        AddPile("Reserve", (i*2), 1, FAN_RIGHT3)
    end

    AddPile("Waste", 2, 3, FAN_DOWN3)
    
    for x = 4, 7 do
        pile = AddPile("Foundation", x, 2, FAN_NONE, MOVE_ANY)
    end

    for x = 4, 7 do
        AddPile("Tableau", x, 3, FAN_DOWN, MOVE_ANY)
    end

end

function StartGame()
    STOCK_RECYCLES = 1
    StockRecycles(STOCK_RECYCLES)
    for _, pile in ipairs(Foundation.Piles) do
        PileLabel(pile, "")
    end
    for _, pile in ipairs(Reserve.Piles) do
        MoveCard(Stock.Pile, pile)
        MoveCard(Stock.Pile, pile)
        MoveCard(Stock.Pile, pile)
    end
    for _, pile in ipairs(Tableau.Piles) do
        MoveCard(Stock.Pile, pile)
    end
    Toast("Choose a Reserve card and move it to a Foundation")
end

-- TailMoveError constraints (Tableau only)

function Tableau.TailMoveError(tail)
    local c1 = Get(tail, 1)
    for i = 2, Len(tail) do
        local c2 = Get(tail, i)
        -- Ranking is continuous in the tableau as Kings can be placed over Aces.
        local err = DownAltColorWrap(c1, c2) if err then return err end
        c1 = c2
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
        if PileLabel(pile) == "" then
            -- To start the game, the player will choose among the top cards of the reserve fans which will start the first foundation pile.
            -- Once he/she makes that decision and picks a card, the three other cards with the same rank, whenever they become available, will start the other three foundations.
            if PileType(CardOwner(c1)) ~= "Reserve" then
                return "The first Foundation card must come from a Reserve"
            end
            for _, pile in ipairs(Foundation.Piles) do
                PileLabel(pile, U[CardOrdinal(c1)])
            end
        end
        if U[CardOrdinal(c1)] ~= PileLabel(pile) then
            return "Foundation can only accept a " .. PileLabel(pile) .. ", not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        -- The foundations are built up by suit and ranking is continuous as Aces are placed over Kings. 
        local err = UpSuitWrap(c1, c2) if err then return err end
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    local c1 = First(tail)
    if Empty(pile) then
        -- Spaces that occur on the tableau are filled with any top card in the reserve.
        -- If the entire reserve is exhausted however, it is not replenished; spaces that occur after this point have to be filled with cards from the waste pile or, if a wastepile has not been made yet, the stock.
        if CardOwner(c1) == Waste.Pile then
            for _, res in ipairs(Reserve.Piles) do
                if Len(res) > 0 then
                    return "An empty Tableau must be filled from a Reserve"
                end
            end
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = DownAltColorWrap(c1, c2) if err then return err end
    end
    return nil
end

function Tableau.UnsortedPairs(pile)
    local unsorted = 0
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = DownAltColorWrap(c1, c2)
        if err then
            unsorted = unsorted + 1
        end
        c1 = c2
    end
    return unsorted
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

function AfterMove()
    -- io.stdout:write("AfterMove\n")
    -- for i = 1, 4 do
    --     if Empty(Tableau.Piles[i]) then
    --         MoveCard(Reserve.Pile, Tableau.Piles[i])
    --     end
    -- end
end
