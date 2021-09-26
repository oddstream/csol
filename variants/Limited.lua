-- Limited

PACKS = 2

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

    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3, "ConformantW", "ChkTrue")

    for x = 5, 12 do
        local pile = AddPile("Foundation", x, 1, FAN_NONE, "ConformantF", "ChkFalse")
        SetAccept(pile, 1)
    end

    for x = 1, 12 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN, "ConformantT", "ConformantT")
        DealUp(pile, 3)
    end

end

function TwoCardsF(cPrev, cThis)
  if cPrev.prone or cThis.prone then
    io.stderr:write("TwoCardsF prone fail\n")
    return false, "Cannot move a face down card"
  end
  if cPrev.suit ~= cThis.suit then
    io.stderr:write("TwoCardsF suit fail\n")
    return false, nil
  end
  if cPrev.ordinal + 1 ~= cThis.ordinal then
    io.stderr:write("TwoCardsF ordinal fail\n")
    return false, nil
  end
  return true
end

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
      ok, err = TwoCardsF(cTop, cards[1])
      if not ok then
        return false, err
      end
    end
  end

  local cPrev = cards[1]
  for n=2, #cards do
    local cThis = cards[n]
    ok, err = TwoCardsF(cPrev, cThis)
    if not ok then
      return false, err
    end
    cPrev = cThis
  end

  return true, nil
end

function TwoCardsT(cPrev, cThis)
  if cPrev.prone or cThis.prone then
    io.stderr:write("TwoCardsT prone fail\n")
    return false, "Cannot move a face down card"
  end
  if cPrev.suit ~= cThis.suit then
    io.stderr:write("TwoCardsT suit fail\n")
    return false, nil
  end
  if cPrev.ordinal ~= cThis.ordinal + 1 then
    io.stderr:write("TwoCardsT ordinal fail\n")
    return false, nil
  end
  return true
end

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
        ok, err = TwoCardsT(cTop, cards[1])
        if not ok then
          return false, err
        end
      end
    end

    local cPrev = cards[1]
    for n=2, #cards do
      local cThis = cards[n]
      ok, err = TwoCardsT(cPrev, cThis)
      if not ok then
        return false, err
      end
      cPrev = cThis
    end

    return true, nil
end

function ConformantW(pile, cards)
  LogTail("ConformantW tail", cards)
  return cards[1].owner == STOCK
end

function ChkFalse(pile, cards)
  LogTail("ChkFalse tail", cards)
  return false, "You cannot do that"
end

function ChkTrue(pile, cards)
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
