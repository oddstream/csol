-- Freecell

PACKS = 1
SUITS = 4
POWERMOVES = true

-- C sets variable 'BAIZE', 'STOCK', FAN_*

function LogCard(title, card)
  if card then
    io.stderr:write(title .. " {ordinal:" .. card.ordinal .. " suit:" .. card.suit .. " color:" .. card.color .. " owner:" .. PileType(card.owner) .. "}\n")
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

function FoundationAccept(pile, cThis)
    if cThis.ordinal ~= 1 then
      return false, "An empty Foundation can only accept an Ace, not a " .. cThis.ordinal
    end
end

function FoundationBuildPair(cPrev, cThis)
    if cPrev.suit ~= cThis.suit then
      -- io.stderr:write("CheckFoundation suit fail\n")
      return false
    end
    if cPrev.ordinal + 1 ~= cThis.ordinal then
      -- io.stderr:write("CheckFoundation ordinal fail\n")
      return false
    end
    return true
end

function TableauAccept(pile, cThis)
    return true
end

function TableauBuildPair(cPrev, cThis)
    if cPrev.color == cThis.color then
      -- io.stderr:write("CheckTableau color fail\n")
      return false
    end
    if cPrev.ordinal ~= cThis.ordinal + 1 then
      -- io.stderr:write("CheckTableau ordinal fail\n")
      return false
    end
    return true
end

function TableauMovePair(cPrev, cThis)
  return TableauBuildPair(cPrev, cThis)
end

function TableauMoveTail(pileLen, tailLen)
    -- io.stderr:write("CheckTableauTail(" .. pileLen .. "," .. tailLen .. ")\n")

    if POWERMOVES or tailLen == 1 then
      return true
    end

    return false, "Can only move one card"
end
