-- Klondike

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
    MovePileTo(STOCK, 100, 50)

    local pile

    pile = AddPile("Waste", 200, 50, FAN_WASTERIGHT, "ChkFalse", "ChkFalse")

    for x = 400, 700, 100 do
        pile = AddPile("Foundation", x, 50, FAN_NONE, "ChkFoundation", "ChkFalse")
        SetAccept(pile, 1)
    end

    pile = AddPile("Tableau", 100, 200, FAN_DOWN, "ChkTableau", "ChkTableau")
    SetAccept(pile, 13)
    DealUp(pile, 1)

    pile = AddPile("Tableau", 200, 200, FAN_DOWN, "ChkTableau", "ChkTableau")
    SetAccept(pile, 13)
    DealDown(pile, 1)
    DealUp(pile, 1)

    pile = AddPile("Tableau", 300, 200, FAN_DOWN, "ChkTableau", "ChkTableau")
    SetAccept(pile, 13)
    DealDown(pile, 2)
    DealUp(pile, 1)

    pile = AddPile("Tableau", 400, 200, FAN_DOWN, "ChkTableau", "ChkTableau")
    SetAccept(pile, 13)
    DealDown(pile, 3)
    DealUp(pile, 1)

    pile = AddPile("Tableau", 500, 200, FAN_DOWN, "ChkTableau", "ChkTableau")
    SetAccept(pile, 13)
    DealDown(pile, 4)
    DealUp(pile, 1)

    pile = AddPile("Tableau", 600, 200, FAN_DOWN, "ChkTableau", "ChkTableau")
    SetAccept(pile, 13)
    DealDown(pile, 5)
    DealUp(pile, 1)

    pile = AddPile("Tableau", 700, 200, FAN_DOWN, "ChkTableau", "ChkTableau")
    SetAccept(pile, 13)
    DealDown(pile, 6)
    DealUp(pile, 1)

end

function ChkFoundation(cards)
    io.stderr:write("ChkFoundation passed a tail of " .. tostring(#cards) .. " cards\n")
    if #cards == 0 then
      io.stderr:write("ChkFoundation passed an empty tail\n")
      return false
    end

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

function ChkTableau(cards)
    -- io.stderr:write("ChkTableau passed a tail of " .. tostring(#cards) .. " cards\n")
    -- for n=1, #cards do
    --     io.stderr:write(tostring(n) .. " ordinal " .. cards[n].ordinal .. " suit " .. cards[n].suit .. " color " .. cards[n].color .. "\n")
    -- end
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

function ChkFalse(cards)
    io.stderr:write("ChkFalse\n")
    return false
end

function ChkTrue(cards)
    io.stderr:write("ChkTrue\n")
    return true
end
