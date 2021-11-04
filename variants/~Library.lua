-- ~Library.lua

--[[
    Think of the error string as an error object, which is a string

    Up|Down|Same|UpDown
    Color|AltColor|Suit|AltSuit|Rank
    Wrap
]]

U = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"}
V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}

function DownAltColor(c1, c2)
    if CardColor(c1) == CardColor(c2) then
        return "Cards must be in alternating colors"
    end
    if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
        return "Cards must be in descending rank"
    end
    return nil
end

function DownAltColorWrap(c1, c2)
    if CardColor(c1) == CardColor(c2) then
        return "Cards must be in alternating colors"
    end
    if CardOrdinal(c1) == 1 and CardOrdinal(c2) == 13 then
        -- King on Ace
    elseif CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
        return "Cards must be in descending rank"
    end
    return nil
end

function UpAltColor(c1, c2)
    if CardColor(c1) == CardColor(c2) then
        return "Cards be in alternating colors"
    end
    if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
        return "Cards must be in ascending rank"
    end
    return nil
end

function UpSuit(c1, c2)
    if CardSuit(c1) ~= CardSuit(c2) then
        return "Cards must be the same suit"
    end
    if CardOrdinal(c1) + 1 ~= CardOrdinal(c2) then
        return "Cards must be in ascending rank"
    end
    return nil
end

function DownSuit(c1, c2)
    if CardSuit(c1) ~= CardSuit(c2) then
        return "Cards must be the same suit"
    end
    if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
        return "Cards must be in descending rank"
    end
    return nil
end

function UpSuitWrap(c1, c2)
    if CardSuit(c1) ~= CardSuit(c2) then
        return "Cards must all be the same suit"
    end
    if CardOrdinal(c1) == 13 and CardOrdinal(c2) == 1 then
        -- Ace on King
    elseif CardOrdinal(c1) == CardOrdinal(c2) - 1 then
        -- up, eg 3 on a 2
    else
        return "Cards must go up in rank (Kings on Aces allowed)"
    end
    return nil
end

function DownSuitWrap(c1, c2)
    if CardSuit(c1) ~= CardSuit(c2) then
        return "Cards must all be the same suit"
    end
    if CardOrdinal(c1) == 1 and CardOrdinal(c2) == 13 then
        -- wrap from Ace to King
    elseif CardOrdinal(c1) - 1 == CardOrdinal(c2) then
        -- down, eg 2 on a 3
    else
        return "Cards must go down in rank (Kings on Aces allowed)"
    end
    return nil
end

function PlusMinusOne(c1, c2)
    if CardOrdinal(c1) == CardOrdinal(c2) + 1 then
        -- build up one
    elseif CardOrdinal(c1) == CardOrdinal(c2) - 1 then
        -- build down one
    else
        return "Cards must be plus/minus one in rank"
    end
    return nil
end

function PlusMinusOneWrap(c1, c2)
    if CardOrdinal(c1) == 1 and CardOrdinal(c2) == 13 then
        -- wrap from Ace to King
    elseif CardOrdinal(c1) == 13 and CardOrdinal(c2) == 1 then
        -- wrap from King to Ace
    elseif CardOrdinal(c1) == CardOrdinal(c2) + 1 then
        -- build up one
    elseif CardOrdinal(c1) == CardOrdinal(c2) - 1 then
        -- build down one
    else
        return "Cards must be plus/minus one in rank"
    end
    return nil
end

function PlusMinusOneSuitWrap(c1, c2)
    if CardSuit(c1) ~= CardSuit(c2) then
        return "Cards must be the same suit"
    elseif CardOrdinal(c1) == 1 and CardOrdinal(c2) == 13 then
        -- wrap from Ace to King
    elseif CardOrdinal(c1) == 13 and CardOrdinal(c2) == 1 then
        -- wrap from King to Ace
    elseif CardOrdinal(c1) == CardOrdinal(c2) + 1 then
        -- build up one
    elseif CardOrdinal(c1) == CardOrdinal(c2) - 1 then
        -- build down one
    else
        return "Cards must be plus/minus one in rank"
    end
    return nil
end
