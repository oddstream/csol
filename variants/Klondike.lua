-- Klondike

PACKS = 1
SUITS = 4
POWERMOVES = false
-- SEED = 3 -- 2 winnable draw three

StockDealCards = 1

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
  
    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)
    
    local pile

    for x = 4, 7 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        SetPileAccept(pile, 1)
        SetPileDraggable(pile, false)
    end

    local deal = 1
    for x = 1, 7 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        SetPileAccept(pile, 13)
        for n = 1, deal do
          local c = MoveCard(STOCK, pile)
          SetCardProne(c, true)
        end
        SetCardProne(PilePeekCard(pile), false)
        deal = deal + 1
    end

end

function StartGame()
  STOCK_RECYCLES = 999
  SetPileRecycles(STOCK, STOCK_RECYCLES)
end

function CheckFoundation(cPrev, cThis)
  if not cPrev then
    if cThis.ordinal ~= 1 then
      return false, "An empty Foundation can only accept an Ace, not a " .. cThis.ordinal
    end
  else
    if cPrev.suit ~= cThis.suit then
      io.stderr:write("CheckFoundation suit fail\n")
      return false, nil
    end
    if cPrev.ordinal + 1 ~= cThis.ordinal then
      io.stderr:write("CheckFoundation ordinal fail\n")
      return false, nil
    end
  end
  return true
end

function CheckTableau(cPrev, cThis)
  -- if cPrev is nil, then we are trying to place cThis onto an empty pile
  if not cPrev then
    if cThis.ordinal == 13 then
      return true
    else
      return false, "An empty Tableau can only accept a King, not a " .. cThis.ordinal
    end
  else
    if cPrev.color == cThis.color then
      io.stderr:write("CheckTableau color fail\n")
      return false, nil
    end
    if cPrev.ordinal ~= cThis.ordinal + 1 then
      io.stderr:write("CheckTableau ordinal fail\n")
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
    for i=1,StockDealCards do
      MoveCard(STOCK, WASTE)
    end
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
      return nil
    end
  elseif pile == WASTE then
    if PileCardCount(STOCK) > 0 then
      MoveCard(STOCK, WASTE)
      return nil
    end
  end
  return nil
end
