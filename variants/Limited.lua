-- Limited

PACKS = 2

-- C sets variable 'BAIZE', 'STOCK', FAN_*

function LogCard(title, card)
  if card then
    io.stderr:write(title .. " {ordinal:" .. card.ordinal .. " suit:" .. card.suit .. " color:" .. card.color .. " owner:" .. Category(card.owner) .. "}\n")
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

    -- local s = FindPile("Stock", 1)
    -- if s == nil then
    --     io.stderr:write("Build cannot find Stock pile\n")
    --     return
    -- else
    --     io.stderr:write("Build found a pile\n")
    --     MovePileTo(s, 10, 100)
    -- end

    -- a stock pile is always created first, and filled with Packs of shuffled cards
    MovePileTo(STOCK, 1, 1)
    SetRecycles(STOCK, 0)

    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3, "ChkWaste", "ChkTrue")

    for x = 5, 12 do
        local pile = AddPile("Foundation", x, 1, FAN_NONE, "ChkFoundation", "ChkFalse")
        SetAccept(pile, 1)
    end

    for x = 1, 12 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN, "ChkTableau", "ChkTableau")
        DealUp(pile, 3)
    end

end

function ChkF(cPrev, cThis)
  if cPrev.prone or cThis.prone then
    io.stderr:write("ChkFoundation prone fail\n")
    return false, "Cannot move a face down card"
  end
  if cPrev.suit ~= cThis.suit then
    io.stderr:write("ChkFoundation suit fail\n")
    return false, "Incorrect suit"
  end
  if cPrev.ordinal + 1 ~= cThis.ordinal then
    io.stderr:write("ChkFoundation ordinal fail\n")
    return false, "Incorrect value"
  end
  return true
end

function ChkFoundation(cTop, cards)

  LogCard("ChkFoundation card", cTop)
  LogTail("ChkFoundation tail", cards)

  if #cards == 0 then
    io.stderr:write("ChkFoundation passed an empty tail\n")
    return false
  end

  local ok, err

  if cTop then
    ok, err = ChkF(cTop, cards[1])
    if not ok then
      return false, err
    end
  end

  local cPrev = cards[1]
  for n=2, #cards do
    local cThis = cards[n]
    ok, err = ChkF(cPrev, cThis)
    if not ok then
      return false, err
    end
    cPrev = cThis
  end

  return true, nil
end

function ChkT(cPrev, cThis)
  if cPrev.prone or cThis.prone then
    io.stderr:write("ChkTableau prone fail\n")
    return false, "Cannot move a face down card"
  end
  if cPrev.suit ~= cThis.suit then
    io.stderr:write("ChkTableau suit fail\n")
    return false, "Incorrect suit"
  end
  if cPrev.ordinal ~= cThis.ordinal + 1 then
    io.stderr:write("ChkTableau ordinal fail\n")
    return false, "Incorrect value"
  end
  return true
end

function ChkTableau(cTop, cards)

    LogCard("ChkTableau card", cTop)
    LogTail("ChkTableau tail", cards)
    
    if #cards > 1 then
      io.stderr:write("ChkTableau tail length fail\n")
      return false, "Can only move a single card"
    end

    local ok, err
  
    if cTop then
      ok, err = ChkT(cTop, cards[1])
      if not ok then
        return false, err
      end
    end

    local cPrev = cards[1]
    for n=2, #cards do
      local cThis = cards[n]
      ok, err = ChkT(cPrev, cThis)
      if not ok then
        return false, err
      end
      cPrev = cThis
    end

    return true, nil
end

function ChkWaste(cTop, cards)
  LogCard("ChkWaste card", cTop)
  LogTail("ChkWaste tail", cards)
  return cards[1].owner == STOCK
end

function ChkFalse(cTop, cards)
  LogCard("ChkFalse card", cTop)
  LogTail("ChkFalse tail", cards)
  return false, "You cannot do that"
end

function ChkTrue(cTop, cards)
  LogCard("ChkTrue card", cTop)
  LogTail("ChkTrue tail", cards)
  return true, nil
end

function CardTapped(card)
  LogCard("CardTapped", card)

  local cardsMoved = false

  if card.owner == STOCK then
    cardsMoved = MoveCard(STOCK, WASTE)
  end

  return cardsMoved, nil
end
