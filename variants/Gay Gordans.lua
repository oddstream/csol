-- Gay Gordans

dofile("variants/~Library.lua")

-- C sets variables 'BAIZE', FAN_*, and tables to hold pile functions and piles

-- https://www.parlettgames.uk/patience/gaygordons.html
-- https://en.wikipedia.org/wiki/Gay_Gordons_(solitaire)

function Wikipedia()
    return "https://en.wikipedia.org/wiki/Gay_Gordons_(solitaire)"
end

function BuildPiles()

    AddPile("Stock", 6, -5, FAN_NONE, 1, 4)    -- hidden
    
    -- Partlett describes five piles of ten cards each,
    -- but that often gets stuck at the beginning,
    -- and is so unlikely to proceed that it's not really a game
    -- so we use the ten piles of five cards each described in the Wikipedia page
    for x = 1, 10 do
        AddPile("Tableau", x, 1, FAN_DOWN)
    end

    AddPile("Foundation", 12, 1, FAN_NONE)

    AddPile("Reserve", 12, 2, FAN_DOWN)

end

function StartGame()

    MoveCard(Stock.Pile, Reserve.Pile)
    MoveCard(Stock.Pile, Reserve.Pile)

    for _, pile in ipairs(Tableau.Piles) do
        for n = 1, 5 do
            MoveCard(Stock.Pile, pile)
        end
    end

    -- "If any of the resultant ten columns contains exactly three Jacks,
    -- exchange the middle one for the upper card of the reserve."
    for _, tabpile in ipairs(Tableau.Piles) do
        local jacks = {}
        for i = 1, Len(tabpile) do
            local c =  Get(tabpile, i)
            if CardOrdinal(c) == 11 then
                table.insert(jacks, i)
            end
        end
        if #jacks == 3 then
            io.stderr:write("Found three jacks in one pile " .. jacks[1] .. jacks[2] .. jacks[3] .."\n")
            local c1 = Get(tabpile, jacks[2])
            local c2 = Get(Reserve.Pile, 1)
            SwapCards(c1, c2)
            break
        end
    end

    LAST_PILE_COUPLED = nil
end

-- TailMoveError constraints (Tableau only)

function Tableau.TailMoveError(tail)
    if Len(tail) > 1 then
        return "Can only move a single card"
    end
    return nil
end

-- TailAppendError constraints

function Foundation.TailAppendError(pile, tail)
    return "You cannot move cards directly to the Foundation"
end

function Tableau.TailAppendError(pile, tail)
    if Empty(pile) then
        return "Cannot couple with an empty pile"
    end
    local c1 = Last(pile)
    local c2 = First(tail)
    -- io.stderr:write("TailAppendError " .. CardOrdinal(c1) .. " and " .. CardOrdinal(c2) .."\n")

    -- "Eliminate any two numerals that total eleven, such as A+10 etc. 
    -- Marry off Kings and Queens in couples, each couple being of two different suits (to avoid inbreeding).
    -- Jacks (Gordons) pair off and exit (or exeunt) together."
    if CardOrdinal(c1) + CardOrdinal(c2) == 11 then
        -- that's perfect
    elseif CardOrdinal(c1) == 13 and CardOrdinal(c2) == 12 and CardSuit(c1) ~= CardSuit(c2) then
        -- that's ok
    elseif CardOrdinal(c1) == 12 and CardOrdinal(c2) == 13 and CardSuit(c1) ~= CardSuit(c2) then
        -- that's ok
    elseif CardOrdinal(c1) == 11 and CardOrdinal(c2) == 11 then
        -- that's ok
    else
        return "Those cards do not make a couple"
    end
    LAST_PILE_COUPLED = pile
end

-- Actions

function Reserve.TailTapped(tail)
    -- do nothing, default is to send to Foundation
end

function Tableau.TailTapped(tail)
    -- do nothing, default is to send to Foundation
end

function AfterMove()
    if LAST_PILE_COUPLED == nil then
        return
    end
    MoveCard(LAST_PILE_COUPLED, Foundation.Pile)
    CardProne(Last(Foundation.Pile), true)
    MoveCard(LAST_PILE_COUPLED, Foundation.Pile)
    CardProne(Last(Foundation.Pile), true)
    LAST_PILE_COUPLED = nil
end
