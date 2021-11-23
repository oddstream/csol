-- Black Hole.lua

--[[
    https://www.parlettgames.uk/patience/blackhole.html

    Interesting because:
    the Foundation is complete when it contains 52 cards, not 13,
    and there are no Tableau piles,
    and it likes fractional pile placement,
    and more exotic fan types.
]]

dofile("variants/~Library.lua")

-- C sets variables 'BAIZE', FAN_*, and tables to hold pile functions

function Wikipedia()
    return "https://en.wikipedia.org/wiki/Black_Hole_(solitaire)"
end

function BuildPiles()

    AddPile("Stock", -5, -5, FAN_NONE, 1, 4)

    AddPile("Foundation", 5, 2.5, FAN_NONE)
    
    local locations = {
        -- David Partlett's screenshot shows the cards in a sort of oval around the black hole
        -- but that takes up too much screen space
        -- hence the boxy thing we have here
        {x=2, y=1, f=FAN_LEFT}, {x=4, y=1, f=FAN_LEFT}, {x=6, y=1, f=FAN_RIGHT}, {x=8, y=1, f=FAN_RIGHT},
        {x=1, y=2, f=FAN_LEFT}, {x=3, y=2, f=FAN_LEFT}, {x=7, y=2, f=FAN_RIGHT}, {x=9, y=2, f=FAN_RIGHT},
        {x=1, y=3, f=FAN_LEFT}, {x=3, y=3, f=FAN_LEFT}, {x=7, y=3, f=FAN_RIGHT}, {x=9, y=3, f=FAN_RIGHT},
        {x=1.5, y=4, f=FAN_LEFT}, {x=3.5, y=4, f=FAN_LEFT}, {x=5, y=4, f=FAN_DOWN}, {x=6.5, y=4, f=FAN_RIGHT}, {x=8.5, y=4, f=FAN_RIGHT}
    }
    if #locations ~= 17 then
        io.stderr:write("There are " .. #locations .. " when there should be 17\n")
    end

    for _, location in ipairs(locations) do
        AddPile("Reserve", location.x, location.y, location.f)
    end
end

function StartGame()

    -- "Put the Ace of spades in the middle of the board as the base or "black hole"."
    MoveCard(Stock.Pile, Foundation.Pile, 1, 3)

    for _, pile in ipairs(Reserve.Piles) do
        for n = 1, 3 do
            MoveCard(Stock.Pile, pile)
        end
    end

    if not Empty(Stock.Pile) then
        io.stderr:write("Oops, Stock still contains " .. Len(Stock.Pile) .. " cards \n")
    end
end

function Foundation.TailAppendError(pile, tail)
    local err = PlusMinusOneWrap(Last(pile), First(tail)) if err then return err end
    return nil
end

-- Actions
