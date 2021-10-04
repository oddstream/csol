-- Freecell

PACKS = 1

POWERMOVES = true

SEED = 1

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
        Property(pile, DRAGGABLE, false)
    end

    for x = 1, 4 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        DealUp(pile, 7)
        SetSingleCardMove(pile, not POWERMOVES)
    end

    for x = 5, 8 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        DealUp(pile, 6)
        SetSingleCardMove(pile, not POWERMOVES)
    end

end

function CheckCellAccept(cThis)
  io.stderr:write("CheckCellAccept\n")
  return true, nil
end

function CheckCell(cPrev, cThis)
  io.stderr:write("CheckCell\n")
  return cPrev == nil, nil
end

function CheckCellMovable(cPrev, cThis)
  io.stderr:write("CheckCellMovable\n")
  return true, nil
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
  return true
end

function CheckTableauMovable(cPrev, cThis)
  return CheckTableau(cPrev, cThis)
end
