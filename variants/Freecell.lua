-- Freecell

PACKS = 1

POWERMOVES = true

-- C sets variable 'BAIZE', 'STOCK', FAN_*

function LogCard(title, card)
  if card then
    io.stderr:write(title .. " {ordinal:" .. card.ordinal .. " suit:" .. card.suit .. " color:" .. card.color .. " owner:" .. PileCategory(card.owner) .. "}\n")
  else
    io.stderr:write(title .. " {nil}\n")
  end
end

function LogTail(title, cards)
  for n=1, #cards do
    LogCard(title, cards[n])
  end
end

function Build()

    if type(AddPile) ~= "function" then
        io.stderr:write("Build cannot find function AddPile\n")
        return
    end

    -- a stock pile is always created first, and filled with Packs of shuffled cards
    MovePileTo(STOCK, 5, -5)  -- hide the stock off screen

    local pile

    for x = 1, 4 do
        pile = AddPile("Cell", x, 1, FAN_NONE)
    end

    for x = 5, 8 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        SetAccept(pile, 1)
    end

    for x = 1, 4 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        DealUp(pile, 7)
    end

    for x = 5, 8 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        DealUp(pile, 6)
    end

end

function CheckCellAccept(cThis)
  return true, nil
end

function CheckCell(cPrev, cThis)
  return cPrev == nil, nil
end

function CheckFoundationAccept(cThis)
  if cThis.ordinal == 1 then
    return true, nil
  else
    return false, "An empty Foundation can only accept an Ace, not a " .. cThis.ordinal
  end
end

function CheckFoundation(cPrev, cThis)
  if cPrev.suit ~= cThis.suit then
    io.stderr:write("CheckFoundation suit fail\n")
    return false, nil
  end
  if cPrev.ordinal + 1 ~= cThis.ordinal then
    io.stderr:write("CheckFoundation ordinal fail\n")
    return false, nil
  end
  return true
end

--[[
function ConformantF(pile, cards)

  LogTail("ConformantF tail", cards)

  if not pile then
    io.stderr:write("ConformantF passed a nil pile\n")
  end

  if #cards == 0 then
    io.stderr:write("ConformantF passed an empty tail\n")
    return false
  end

  local ok, err

  if pile then
    local cTop = PeekCard(pile)
    if cTop then
      ok, err = CheckFoundationBuild(cTop, cards[1])
      if not ok then
        return false, err
      end
    end
  end

  local cPrev = cards[1]
  for n=2, #cards do
    local cThis = cards[n]
    ok, err = CheckFoundationBuild(cPrev, cThis)
    if not ok then
      return false, err
    end
    cPrev = cThis
  end

  return true, nil
end
]]

function CheckTableauAccept(cThis)
  return true, nil
end

function CheckTableau(cPrev, cThis)
  if cPrev.color == cThis.color then
    io.stderr:write("CheckTableau color fail\n")
    return false, nil
  end
  if cPrev.ordinal ~= cThis.ordinal + 1 then
    io.stderr:write("CheckTableau ordinal fail\n")
    return false, nil
  end
  if PileCategory(cThis.owner) == "Foundation" then
    return false, "Cannot move a card from a Foundation to a Tableau"
  end
  return true
end

function CheckTableauMovable(cPrev, cThis)
  return CheckTableau(cPrev, cThis)
end

--[[
function ConformantT(pile, cards)

  LogTail("ConformantT tail", cards)
  
  if #cards > 1 then
    powerMoves = PowerMoves(BAIZE, pile)
    if powerMoves == 0 then
      return false, "Not enough room to move " .. #cards
    end
    if #cards > powerMoves then
      return false, "Can move " .. powerMoves .. " cards, not " .. #cards
    end
  end

  local ok, err

  if pile then
    local cTop = PeekCard(pile)
    if cTop then
      ok, err = CheckTableauBuild(cTop, cards[1])
      if not ok then
        return false, err
      end
    end
  end

  local cPrev = cards[1]
  for n=2, #cards do
    local cThis = cards[n]
    ok, err = CheckTableauBuild(cPrev, cThis)
    if not ok then
      return false, err
    end
    cPrev = cThis
  end

  return true, nil
end
]]
