-- Baker's Dozen

--[[
    Style
    
    Lua globals are in UPPER_CASE, and use underscores between words
    function names are in MixedCase
]]

dofile("variants/~Library.lua")

-- C sets variables 'BAIZE', FAN_*, and tables to hold pile functions

local function DemoteCards(pile, ord)
    local hasChanged
    local itemCount = Len(pile)
    repeat
        hasChanged = false
        itemCount = itemCount - 1
        for i = 1, itemCount do
            local c1 = Get(pile, i)
            local c2 = Get(pile, i + 1)
            if CardOrdinal(c1) ~= ord and CardOrdinal(c2) == ord then
                SwapCards(c1, c2)
                hasChanged = true
            end
        end
    until not hasChanged 
end

function BuildPiles()
    -- a stock pile is always created first, and filled with PACKS of shuffled cards
    AddPile("Stock", -5, -5, FAN_NONE, 1, 4)

    for x = 1, 7 do
        local pile = AddPile("Tableau", x, 1, FAN_DOWN, MOVE_ONE)
        PileLabel(pile, "X")
    end
    for x = 1, 6 do
        local pile = AddPile("Tableau", x, 4, FAN_DOWN, MOVE_ONE)
        PileLabel(pile, "X")
    end
    for y = 1, 4 do
        local pile = AddPile("Foundation", 8.5, y, FAN_NONE)
        PileLabel(pile, U[1])
    end
end

function StartGame()
    for _, pile in ipairs(Tableau.Piles) do 
        for n = 1, 4 do
            MoveCard(Stock.Pile, pile)
        end
        DemoteCards(pile, 13)
        Refan(pile)
    end

    if not Empty(Stock.Pile) then
        io.stderr:write("Oops! There are still " .. Len(Stock.Pile) .. " cards in the Stock\n")
    end
end

-- TailMoveError

function Tableau.TailMoveError(tail)
    if Len(tail) > 1 then
        return "One one card can be moved"
    end
    return nil
end

-- TailAppendError constraints

function Foundation.TailAppendError(pile, tail)
    if Empty(pile) then
        local c1 = First(tail)
        if CardOrdinal(c1) ~= 1 then
            return "Foundation can only accept an Ace, not a " .. V[CardOrdinal(c1)]
        end
    else
        local err = UpSuit(Last(pile), First(tail)) if err then return err end
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    if Empty(pile) then
        return "You cannot move a card to an empty pile"
    else
        local err = Down(Last(pile), First(tail)) if err then return err end
    end
    return nil
end

-- PileConformantError (Tableau only)

function Tableau.PileConformantError(pile)
    for _, c in ipairs(CardPairs(pile)) do
        local err = Down(c[1], c[2]) if err then return err end
    end
    return nil
end

-- SortedAndUnSorted (Tableau only)

function Tableau.SortedAndUnsorted(pile)
    local sorted = 0
    local unsorted = 0
    for _, c in ipairs(CardPairs(pile)) do
        local err = Down(c[1], c[2])
        if err then
            unsorted = unsorted + 1
        else
            sorted = sorted + 1
        end
    end
    return sorted, unsorted
end

-- Actions
