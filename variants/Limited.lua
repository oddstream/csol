-- Limited

PACKS = 2
SUITS = 4
POWERMOVES = true
-- SEED = 4  -- winnable

-- C sets variables 'BAIZE', 'STOCK', FAN_*

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
    PileMoveTo(STOCK, 1, 1)
    SetPileRecycles(STOCK, 0)

    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)

    for x = 5, 12 do
        local pile = AddPile("Foundation", x, 1, FAN_NONE)
        SetPileAccept(pile, 1)
        SetPileDraggable(pile, false)
    end

    for x = 1, 12 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN)
        for n=1,3 do
          MoveCard(STOCK, pile)
        end
    end

end

function CheckFoundation(cPrev, cThis)
  if not cPrev then
    if cThis.ordinal ~= 1 then
      return false, "An empty Foundation can only accept an Ace, not a " .. cThis.ordinal
    end
  else
    if cPrev.suit ~= cThis.suit then
      -- io.stderr:write("CheckFoundation suit fail\n")
      return false, nil
    end
    if cPrev.ordinal + 1 ~= cThis.ordinal then
      -- io.stderr:write("CheckFoundation ordinal fail\n")
      return false, nil
    end
  end
  return true
end

function CheckTableau(cPrev, cThis)
  if not cPrev then
    -- accept any card to an empty pile
  else
    if cPrev.suit ~= cThis.suit then
      -- io.stderr:write("CheckTableau suit fail\n")
      return false, nil
    end
    if cPrev.ordinal ~= cThis.ordinal + 1 then
      -- io.stderr:write("CheckTableau ordinal fail\n")
      return false, nil
    end
  end
  return true
end

function CheckTableauMovable(cPrev, cThis)
  return CheckTableau(cPrev, cThis)
end

function CheckTableauTail(pileLen, tailLen)
  io.stderr:write("CheckTableauTail(" .. pileLen .. "," .. tailLen .. ")\n")

  if POWERMOVES or tailLen == 1 then
    return true, nil
  end

  return false, "Can only move one card"
end

function CardTapped(card)
  -- LogCard("CardTapped", card)
  if card.owner == STOCK then
    MoveCard(STOCK, WASTE)
  end
end

function PileTapped(pile)
  if pile == WASTE then
    if PileCardCount(STOCK) > 0 then
      MoveCard(STOCK, WASTE)
    end
  end
end
