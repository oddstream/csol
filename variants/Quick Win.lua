-- Quick Win

PACKS = 1
SUITS = 4
POWERMOVES = false
STOCK_RECYCLES = 1

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
    PileMoveTo(STOCK, 1, 1)

    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)

    for x = 7, 10 do
        local pile = AddPile("Foundation", x, 1, FAN_NONE)
        SetPileAccept(pile, 1)
        SetPileDraggable(pile, false)
    end

    TABLEAUX = {}

    for x = 1, 10 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN)
        for n=1,2 do
          MoveCard(STOCK, pile)
        end
        PileDemoteCards(pile, 13)
        PilePromoteCards(pile, 1)
        table.insert(TABLEAUX, pile)
    end

    for x = 1, 10 do
        local pile = AddPile("Tableau", x, 4, FAN_DOWN)
        for n=1,2 do
          MoveCard(STOCK, pile)
        end
        PileDemoteCards(pile, 13)
        PilePromoteCards(pile, 1)
        table.insert(TABLEAUX, pile)
    end
end

function StartGame()
  STOCK_RECYCLES = 1
  SetPileRecycles(STOCK, STOCK_RECYCLES)
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
    -- io.stderr:write("CheckFoundation suit fail\n")
    return false, nil
  end
  if cPrev.ordinal + 1 ~= cThis.ordinal then
    -- io.stderr:write("CheckFoundation ordinal fail\n")
    return false, nil
  end
  return true
end

function CheckTableauAccept(cThis)
  if cThis.prone then
    return false, "Cannot move a face down card"
  end
  return true, nil
end

function CheckTableau(cPrev, cThis)
  if cPrev.ordinal ~= cThis.ordinal + 1 then
    -- io.stderr:write("CheckTableau ordinal fail\n")
    return false, nil
  end
  return true
end

function CheckTableauMovable(cPrev, cThis)
  return CheckTableau(cPrev, cThis)
end

function CardTapped(card)
  LogCard("CardTapped", card)

  local cardsMoved = false

  if card.owner == STOCK then
    cardsMoved = MoveCard(STOCK, WASTE)
  end

  return cardsMoved, nil
end

function PileTapped(pile)
  io.stdout:write("PileTapped\n")
  if pile == STOCK then
    if STOCK_RECYCLES == 0 then
      return false, "No more Stock recycles"
    end
    if PileCardCount(WASTE) > 0 then
      while PileCardCount(WASTE) > 0 do
        MoveCard(WASTE, STOCK)
      end
      STOCK_RECYCLES = STOCK_RECYCLES - 1
      SetPileRecycles(STOCK, STOCK_RECYCLES)
      return true, nil
    end
  elseif pile == WASTE then
    if PileCardCount(STOCK) > 0 then
      MoveCard(STOCK, WASTE)
      return true, nil
    end
  end

  return false, nil
end
  
function AfterMove()
  -- io.stdout:write("AfterMove\n")
  for _, pile in ipairs(TABLEAUX) do
    if PileCardCount(pile) == 0 then
      if PileCardCount(WASTE) > 0 then
        MoveCard(WASTE, pile)
      elseif PileCardCount(STOCK) > 0 then
        MoveCard(STOCK, pile)
      end
    end
  end
  if PileCardCount(WASTE) == 0 then
    MoveCard(STOCK, WASTE)
  end
end
