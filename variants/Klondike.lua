-- Klondike

PACKS = 1
POWERMOVES = false
SEED = 2 -- winnable draw three

-- C sets variables 'BAIZE', 'STOCK', FAN_*

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
    MovePileTo(STOCK, 1, 1)
    SetRecycles(STOCK, 9999)
  
    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)
    
    local pile

    for x = 4, 7 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        SetAccept(pile, 1)
        Property(pile, DRAGGABLE, false)
    end

    local dealDown = 0
    for x = 1, 7 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        SetSingleCardMove(pile, false)
        SetAccept(pile, 13)
        DealDown(pile, dealDown)
        dealDown = dealDown + 1
        DealUp(pile, 1)
    end

    Property(pile, DRAGGABLE, false)
    local draggable = Property(pile, DRAGGABLE)
    if draggable then
      io.stderr:write("DRAGGABLE: {true}\n")
    else
      io.stderr:write("DRAGGABLE: {false or nil}\n")
    end
    
    Property(pile, DRAGGABLE, true)
    draggable = Property(pile, DRAGGABLE)
    if draggable then
      io.stderr:write("DRAGGABLE: {true}\n")
    else
      io.stderr:write("DRAGGABLE: {false or nil}\n")
    end

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
  if cThis.ordinal == 13 then
    return true, nil
  else
    return false, "An empty Tableau can only accept a King, not a " .. cThis.ordinal
  end
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

function CardTapped(card)
  LogCard("CardTapped", card)

  local cardsMoved = 0

  if card.owner == STOCK then
    for i=1,3 do
      if MoveCard(STOCK, WASTE) then
        cardsMoved = cardsMoved + 1
      end
    end
  end

  return cardsMoved > 0, nil
end
