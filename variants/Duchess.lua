-- Duchess

--[[
    https://en.wikipedia.org/wiki/Duchess_(solitaire)
    Duchess (also Dutchess) is a patience or solitaire card game which uses a deck of 52 playing cards.
    It has all four typical features of a traditional solitaire game: a tableau, a reserve, a stock and a waste pile, and is quite easy to win.
    It is closely related to Canfield.
]]

dofile("variants/~Library.lua")

POWER_MOVES = false
STOCK_DEAL_CARDS = 1

function BuildPiles()

    STOCK = AddPile("Stock", 2, 2, FAN_NONE, 1, 4)
  
    local pile

    RESERVES = {}
    for i = 1, 4 do
        pile = AddPile("Reserve", (i*2), 1, FAN_RIGHT3)
        table.insert(RESERVES, pile)
        for j = 1, 3 do
            MoveCard(STOCK, pile)
        end
    end

    WASTE = AddPile("Waste", 2, 3, FAN_DOWN3)
    
    FOUNDATIONS = {}
    for x = 4, 7 do
        pile = AddPile("Foundation", x, 2, FAN_NONE)
        table.insert(FOUNDATIONS, pile)
    end

    TABLEAUX = {}
    for x = 4, 7 do
        pile = AddPile("Tableau", x, 3, FAN_DOWN)
        table.insert(TABLEAUX, pile)
        MoveCard(STOCK, pile)
    end

end

function StartGame()
    STOCK_RECYCLES = 1
    for _, pile in ipairs(FOUNDATIONS) do
        PileAccept(pile, 0)
    end
end

-- CanTailBeMoved constraints (Tableau only)

function Tableau.CanTailBeMoved(tail)
    local c1 = Get(tail, 1)
    for i = 2, Len(tail) do
        local c2 = Get(tail, i)
        -- Ranking is continuous in the tableau as Kings can be placed over Aces.
        local err = DownAltColorWrap(c1, c2) if err then return false, err end
        c1 = c2
    end
    return true
end

-- CanTailBeAppended constraints

function Waste.CanTailBeAppended(pile, tail)
    if CardOwner(First(tail)) ~= STOCK then
        return false, "The Waste can only accept cards from the Stock"
    end
    return true
end

function Foundation.CanTailBeAppended(pile, tail)
    if Empty(pile) then
        local c1 = First(tail)
        if PileAccept(pile) == 0 then
            -- To start the game, the player will choose among the top cards of the reserve fans which will start the first foundation pile.
            -- Once he/she makes that decision and picks a card, the three other cards with the same rank, whenever they become available, will start the other three foundations.
            if PileType(CardOwner(c1)) ~= "Reserve" then
                return false, "The first Foundation card must come from a Reserve"
            end
            for _, pile in ipairs(FOUNDATIONS) do
                PileAccept(pile, CardOrdinal(c1))
            end
        end
        if CardOrdinal(c1) ~= PileAccept(pile) then
            return false, "Foundation can only accept a " .. V[PileAccept(pile)] .. ", not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        -- The foundations are built up by suit and ranking is continuous as Aces are placed over Kings. 
        local err = UpSuitWrap(c1, c2) if err then return false, err end
    end
    return true
end

function Tableau.CanTailBeAppended(pile, tail)
    local c1 = First(tail)
    if Empty(pile) then
        -- Spaces that occur on the tableau are filled with any top card in the reserve.
        -- If the entire reserve is exhausted however, it is not replenished; spaces that occur after this point have to be filled with cards from the waste pile or, if a wastepile has not been made yet, the stock.
        if PileType(CardOwner(c1)) == "Waste" then
            for _, res in ipairs(RESERVES) do
                if Len(res) > 0 then
                    return false, "An empty Tableau must be filled from a Reserve"
                end
            end
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = DownAltColorWrap(c1, c2) if err then return false, err end
    end
    return true
end

-- IsPileConformant

function Tableau.IsPileConformant(pile)
    local c1 = First(pile)
    for i = 2, Len(pile) do
        local c2 = Get(pile, n)
        local err = DownAltColorWrap(c1, c2) if err then return false, err end
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
        local err = DownAltColorWrap(c1, c2)
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

function Stock.Tapped(tail)
    if tail == nil then
        if STOCK_RECYCLES == 0 then
            Toast("No more Stock recycles")
            return
        end
        if Len(WASTE) > 0 then
            while Len(WASTE) > 0 do
                MoveCard(WASTE, STOCK)
            end
            STOCK_RECYCLES = STOCK_RECYCLES - 1
          end
      else
        for i = 1, STOCK_DEAL_CARDS do
            MoveCard(STOCK, WASTE)
        end
    end
end

function AfterMove()
    -- io.stdout:write("AfterMove\n")
    -- for i = 1, 4 do
    --     if Empty(TABLEAUX[i]) then
    --         MoveCard(RESERVE, TABLEAUX[i])
    --     end
    -- end
end
