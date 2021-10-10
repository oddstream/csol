-- Quick Win

PACKS = 1
SUITS = 4
POWERMOVES = false
STOCK_RECYCLES = 1
-- CARD_FILTER = {1, 7, 8, 9, 10, 11, 12}  -- twos, threes, fours, fives, sixes, and kings removed
STRIP_CARDS = {12,13}

-- C sets variables 'BAIZE', 'STOCK', FAN_*

function LogCard(title, card)
  if card and type(card) == "table" then
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

    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)

    for x = 7, 10 do
        local pile = AddPile("Foundation", x, 1, FAN_NONE)
        SetPileAccept(pile, 1)
        SetPileDraggable(pile, false)
    end

    TABLEAUX = {}

    for x = 1, 10 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN)
        for n = 1, 2 do
          MoveCard(STOCK, pile)
        end
        PileDemoteCards(pile, 13)
        PilePromoteCards(pile, 1)
        table.insert(TABLEAUX, pile)
    end

    for x = 1, 10 do
        local pile = AddPile("Tableau", x, 4, FAN_DOWN)
        for n = 1, 2 do
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
    if cPrev.ordinal ~= cThis.ordinal + 1 then
      -- io.stderr:write("CheckTableau ordinal fail\n")
      return false, nil
    end
    if cPrev.suit ~= cThis.suit then
      return false, nil
    end
  end
  return true
end

function CheckTableauMovable(cPrev, cThis)
  return CheckTableau(cPrev, cThis)
end

function CardTapped(card)
  -- LogCard("CardTapped", card)
  if card.owner == STOCK then
    MoveCard(STOCK, WASTE)
  end
end

function PileTapped(pile)
  -- io.stdout:write("PileTapped\n")
  if pile == STOCK then
    if STOCK_RECYCLES == 0 then
      return "No more Stock recycles"
    end
    if PileCardCount(WASTE) > 0 then
      while PileCardCount(WASTE) > 0 do
        MoveCard(WASTE, STOCK)
      end
      STOCK_RECYCLES = STOCK_RECYCLES - 1
      SetPileRecycles(STOCK, STOCK_RECYCLES)
    end
  elseif pile == WASTE then
    if PileCardCount(STOCK) > 0 then
      MoveCard(STOCK, WASTE)
    end
  end
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
