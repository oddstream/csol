-- Freecell

PACKS = 1

-- C sets variable 'BAIZE', 'STOCK', FAN_*

function Build()

    if type(AddPile) ~= "function" then
        io.stderr:write("Build cannot find function AddPile\n")
        return
    end

    -- local s = FindPile("Stock", 1)
    -- if s == nil then
    --     io.stderr:write("Build cannot find Stock pile\n")
    --     return
    -- else
    --     io.stderr:write("Build found a pile\n")
    --     MovePileTo(s, 10, 100)
    -- end

    -- a stock pile is always created first, and filled with Packs of shuffled cards
    MovePileTo(STOCK, 5, -5)  -- hide the stock off screen

    local pile

    for x = 1, 4 do
        pile = AddPile("Cell", x, 1, FAN_NONE, "ChkTrue", "ChkTrue")
    end

    for x = 5, 8 do
        pile = AddPile("Foundation", x, 1, FAN_NONE, "ChkFoundation", "ChkFalse")
        SetAccept(pile, 1)
    end

    for x = 1, 4 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN, "ChkTableau", "ChkTableau")
        DealUp(pile, 7)
    end

    for x = 5, 8 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN, "ChkTableau", "ChkTableau")
        DealUp(pile, 6)
    end

end

function ChkFoundation(source, cards)
    -- io.stderr:write("ChkFoundation passed a tail of " .. tostring(#cards) .. " cards\n")
    -- if #cards == 0 then
    --   io.stderr:write("ChkFoundation passed an empty tail\n")
    --   return false
    -- end

    local cPrev = cards[1]
    for n=2, #cards do
      local cThis = cards[n]
      if cPrev.prone or cThis.prone then
        io.stderr:write("ChkFoundation prone fail\n")
        return false
      end
      if cPrev.suit ~= cThis.suit then
        io.stderr:write("ChkFoundation suit fail\n")
        return false
      end
      if cPrev.ordinal + 1 ~= cThis.ordinal then
        io.stderr:write("ChkFoundation ordinal fail\n")
        return false
      end
      cPrev = cThis
    end
    return true
end

function ChkTableau(source, cards)
    -- io.stderr:write("ChkTableau passed a tail of " .. tostring(#cards) .. " cards\n")
    -- for n=1, #cards do
    --     io.stderr:write(tostring(n) .. " ordinal " .. cards[n].ordinal .. " suit " .. cards[n].suit .. " color " .. cards[n].color .. "\n")
    -- end
    if #cards > 2 then
        io.stderr:write("ChkTableau tail length fail\n")
        return false
    end
    if source == "Foundation" then
      io.stderr:write("ChkTableau coming from Foundation fail\n")
      return false
    end

    local cPrev = cards[1]
    for n=2, #cards do
      local cThis = cards[n]
      if cPrev.prone or cThis.prone then
        io.stderr:write("ChkTableau prone fail\n")
        return false
      end
      if cPrev.color == cThis.color then
        io.stderr:write("ChkTableau color fail\n")
        return false
      end
      if cPrev.ordinal ~= cThis.ordinal + 1 then
        io.stderr:write("ChkTableau ordinal fail\n")
        return false
      end
      cPrev = cThis
    end
    return true
end

function ChkFalse(source, cards)
    io.stderr:write("ChkFalse\n")
    return false
end

function ChkTrue(source, cards)
    io.stderr:write("ChkTrue\n")
    return true
end
