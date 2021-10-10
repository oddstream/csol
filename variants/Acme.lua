-- Acme

--[[
  Acme is a Canfield type of patience or solitaire card game using a single deck of playing cards.
  Acme has four tableau stacks of one card each, and they are built down in suit.
  There are also four Foundations that build up in suit.
  The Reserve Pile contains 13 cards which can be played onto the Foundations or Tableau Stacks.
  The deck turns up one card at a time.
  Only the top card of a Tableau stack can be moved.
  These cards can be moved to a Foundation or onto another Tableau stack.
  The Tableau builds down in suit, and the Foundations build up in suit.
  Cards from the Reserve automatically fill empty spaces.
  Any card can fill empty Tableau spaces after the Reserve is empty.
  There is only one redeal allowed in this game, so only two passes through the deck are allowed.
  Strategy: Rather than using the cards from the deck, a player should try to use all of the reserve cards first.
  Only two passes are allowed, so the deck should be used wisely.
]]

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

    RESERVE = AddPile("Reserve", 1, 2, FAN_DOWN)
    for n = 1, 13 do
        local c = MoveCard(STOCK, RESERVE)
        SetCardProne(c, true)
    end
    SetCardProne(PilePeekCard(RESERVE), false)

    TABLEAUX = {}
    for x = 4, 7 do
        pile = AddPile("Tableau", x, 2, FAN_DOWN)
        MoveCard(STOCK, pile)
        TABLEAUX[#TABLEAUX+1] = pile
    end

end

function StartGame()
  STOCK_RECYCLES = 1
  SetPileRecycles(STOCK, STOCK_RECYCLES)
end

function FoundationAccept(pile, cThis)
  if cThis.ordinal ~= 1 then
    return false, "An empty Foundation can only accept an Ace, not a " .. cThis.ordinal
  end
  return true
end

function FoundationBuildPair(cPrev, cThis)
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

function TableauAccept(pile, cThis)
    -- accept any card to an empty pile
    return true
end

function TableauBuildPair(cPrev, cThis)
    if cPrev.suit ~= cThis.suit then
      -- io.stderr:write("CheckTableau suit fail\n")
      return false, nil
    end
    if cPrev.ordinal ~= cThis.ordinal + 1 then
      -- io.stderr:write("CheckTableau ordinal fail\n")
      return false, nil
    end
    return true
end

function TableauMovePair(cPrev, cThis)
    return TableauBuildPair(cPrev, cThis)
end

function TableauMoveTail(pileLen, tailLen)
    io.stderr:write("CheckTableauTail(" .. pileLen .. "," .. tailLen .. ")\n")

    if POWERMOVES or tailLen == 1 then
      return true, nil
    end

    return false, "Can only move one card"
end

function CardTapped(card)
  -- LogCard("CardTapped", card)
  if card.owner == STOCK then
    for i = 1, StockDealCards do
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

function AfterMove()
    -- io.stdout:write("AfterMove\n")
    for i = 1, 4 do
        if PileCardCount(TABLEAUX[i]) == 0 then
            MoveCard(RESERVE, TABLEAUX[i])
        end
    end
end
