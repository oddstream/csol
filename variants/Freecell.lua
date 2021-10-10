-- Freecell

PACKS = 1
SUITS = 4
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
  PileMoveTo(STOCK, 5, -5)  -- hide the stock off screen

  local pile

  for x = 1, 4 do
    pile = AddPile("Cell", x, 1, FAN_NONE)
  end

  for x = 5, 8 do
    pile = AddPile("Foundation", x, 1, FAN_NONE)
    SetPileAccept(pile, 1)
    SetPileDraggable(pile, false);
  end

  for x = 1, 4 do
    pile = AddPile("Tableau", x, 2, FAN_DOWN)
    for n = 1, 7 do
      MoveCard(STOCK, pile)
    end
    if EASY then
      PileDemoteCards(pile, 13)
      PilePromoteCards(pile, 1)
    end
  end

  for x = 5, 8 do
    pile = AddPile("Tableau", x, 2, FAN_DOWN)
    for n = 1, 6 do
      MoveCard(STOCK, pile)
    end
    if EASY then
      PileDemoteCards(pile, 13)
      PilePromoteCards(pile, 1)
    end
  end

end

-- function CheckCell(cPrev, cThis)
--   io.stderr:write("CheckCell\n")
--   return cPrev == nil, nil
-- end

-- function CheckCellMovable(cPrev, cThis)
--   io.stderr:write("CheckCellMovable\n")
--   return true, nil
-- end

-- function CheckCellTail(pileLen, tailLen)
--   io.stderr:write("CheckCellTail\n")
--   return true
-- end

function CheckFoundation(cPrev, cThis)
  if not cPrev then
    if cThis.ordinal ~= 1 then
      return false, "An empty Foundation can only accept an Ace, not a " .. cThis.ordinal
    end
  else
    if cPrev.suit ~= cThis.suit then
      -- io.stderr:write("CheckFoundation suit fail\n")
      return false
    end
    if cPrev.ordinal + 1 ~= cThis.ordinal then
      -- io.stderr:write("CheckFoundation ordinal fail\n")
      return false
    end
  end
  return true
end

function CheckTableauAccept(cThis)
  if cThis.prone then
    return false, "Cannot move a face down card"
  end
  return true
end

function CheckTableau(cPrev, cThis)
  if not cPrev then
    -- accept any card to an empty pile
  else
    if cPrev.color == cThis.color then
      -- io.stderr:write("CheckTableau color fail\n")
      return false
    end
    if cPrev.ordinal ~= cThis.ordinal + 1 then
      -- io.stderr:write("CheckTableau ordinal fail\n")
      return false
    end
  end
  return true
end

function CheckTableauMovable(cPrev, cThis)
  return CheckTableau(cPrev, cThis)
end

function CheckTableauTail(pileLen, tailLen)
  -- io.stderr:write("CheckTableauTail(" .. pileLen .. "," .. tailLen .. ")\n")

  if POWERMOVES or tailLen == 1 then
    return true
  end

  return false, "Can only move one card"
end

