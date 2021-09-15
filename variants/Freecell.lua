-- Freecell

Packs = 1

-- C sets variable 'BAIZE', FAN_*

function Build()

    if type(AddPile) ~= "function" then
        io.stderr:write("Build cannot find function AddPile\n")
        return
    end

    local s = FindPile("Stock", 1)
    if s == nil then
        io.stderr:write("Build cannot find Stock pile\n")
        return
    else
        io.stderr:write("Build found a pile\n")
        MovePileTo(s, 10, 100)
    end

    local pile

    for x = 100, 400, 100 do
        pile = AddPile("Cell", x, 100, FAN_NONE, "ChkTrue", "ChkTrue")
    end

    for x = 500, 800, 100 do
        pile = AddPile("Foundation", x, 100, FAN_NONE, "ChkFoundation", "ChkFalse")
        SetAccept(pile, 1)
    end

    for x = 100, 400, 100 do
        pile = AddPile("Tableau", x, 300, FAN_DOWN, "ChkTableau", "ChkTableau")
        DealUp(pile, 7)
    end

    for x = 500, 800, 100 do
        pile = AddPile("Tableau", x, 300, FAN_DOWN, "ChkTableau", "ChkTableau")
        DealUp(pile, 6)
    end

end

function ConformantPair(cPrev, cThis)
    -- cards have .suit, .ordinal, .color, .prone
    if cPrev.prone or cThis.prone then
        return false
    end
    return false
end

function ConformantTail(cards)
    local cPrev = cards[1]
    for n=2, #cards do
      local cThis = cards[n]
      if not ConformantPair(cPrev, cThis) then
        return false
      end
      cPrev = cThis
    end
    return true
end

function ChkFoundation(cards)
    io.stderr:write("ChkFoundation passed a tail of " .. tostring(#cards) .. " cards\n")
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
    io.stderr:write("ChkTableau passed a tail of " .. tostring(#cards) .. " cards\n")
    for n=1, #cards do
        io.stderr:write(tostring(n) .. " ordinal " .. cards[n].ordinal .. " suit " .. cards[n].suit .. " color " .. cards[n].color .. "\n")
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

function ChkFalse(cards)
    io.stderr:write("ChkFalse\n")
    return false
end

function ChkTrue(cards)
    io.stderr:write("ChkTrue\n")
    return true
end
