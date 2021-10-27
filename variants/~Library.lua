-- ~Library.lua

--[[
    Think of the error string as an error object, which is a string

    Up|Down|Same|UpDown
    Color|AltColor|Suit|AltSuit|Rank
    Wrap
]]

function First(thing)
    return Get(thing, 1)
end

function Last(thing)
    return Peek(thing)
end

function DownAltColor(c1, c2)
    if CardColor(c1) == CardColor(c2) then
        return "Cards must be in alternating colors"
    end
    if CardOrdinal(c1) ~= CardOrdinal(c2) + 1 then
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
