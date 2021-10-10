-- Duchess

--[[
    Duchess (also Dutchess) is a patience or solitaire card game which uses a deck of 52 playing cards.
    It has all four typical features of a traditional solitaire game: a tableau, a reserve, a stock and a waste pile, and is quite easy to win.
    It is closely related to Canfield.
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

    RESERVES = {}
    for i = 1, 4 do
        local pile = AddPile("Reserve", (i*2), 1, FAN_RIGHT3)
        RESERVES[#RESERVES+1] = pile
        for j = 1, 3 do
            MoveCard(STOCK, pile)
        end
    end

    -- a stock pile is always created first, and filled with Packs of shuffled cards
    PileMoveTo(STOCK, 2, 2)
  
    WASTE = AddPile("Waste", 2, 3, FAN_DOWN3)
    
    FOUNDATIONS = {}
    for x = 4, 7 do
        local pile = AddPile("Foundation", x, 2, FAN_NONE)
        FOUNDATIONS[#FOUNDATIONS+1] = pile
        SetPileDraggable(pile, false)
    end

    TABLEAUX = {}
    for x = 4, 7 do
        local pile = AddPile("Tableau", x, 3, FAN_DOWN)
        TABLEAUX[#TABLEAUX+1] = pile
        MoveCard(STOCK, pile)
    end

end

function StartGame()
    STOCK_RECYCLES = 1
    SetPileRecycles(STOCK, STOCK_RECYCLES)
    FOUNDATION_ACCEPT = 0
end

function CheckFoundation(cPrev, cThis)
    if not cPrev then
        -- To start the game, the player will choose among the top cards of the reserve fans which will start the first foundation pile.
        -- Once he/she makes that decision and picks a card, the three other cards with the same rank, whenever they become available, will start the other three foundations.
        if FOUNDATION_ACCEPT == 0 then
            -- LogCard("Faccept", cThis)
            if PileType(cThis.owner) ~= "Reserve" then
                return false, "The first Foundation card must come from a Reserve"
            end
            FOUNDATION_ACCEPT = cThis.ordinal
            for _, pile in ipairs(FOUNDATIONS) do
                SetPileAccept(pile, FOUNDATION_ACCEPT)
            end
        end
        if cThis.ordinal == FOUNDATION_ACCEPT then
            return true, nil
        else
            return false, "An empty Foundation can only accept a " .. FOUNDATION_ACCEPT .. ", not a " .. cThis.ordinal
        end
    else
        -- The foundations are built up by suit and ranking is continuous as Aces are placed over Kings.
        if cPrev.suit ~= cThis.suit then
            return false, nil
        end
        if cPrev.ordinal == 13 and cThis.ordinal == 1 then
            -- wrap from King to Ace
            return true, nil
        elseif cPrev.ordinal + 1 == cThis.ordinal then
            -- up, eg 2 to 3
            return true, nil
        else
            return false, nil
        end
    end
    return true
end

function CheckTableau(cPrev, cThis)
    if not cPrev then
        -- Spaces that occur on the tableau are filled with any top card in the reserve.
        -- If the entire reserve is exhausted however, it is not replenished; spaces that occur after this point have to be filled with cards from the waste pile or, if a wastepile has not been made yet, the stock.
        if PileType(cThis.owner) == "Waste" then
            for _, pile in ipairs(RESERVES) do
                if PileCardCount(pile) > 0 then
                    return false, "An empty Tableau must be filled from a Reserve"
                end
            end
        end
    else
        -- The cards on the tableau are built down in alternating colors. Ranking is also continuous in the tableau as Kings can be placed over Aces. 
        if cPrev.color == cThis.color then
            -- io.stderr:write("CheckTableau color fail\n")
            return false, nil
        end
        if cPrev.ordinal == 1 and cThis.ordinal == 13 then
        -- wrap from Ace to King
        elseif cPrev.ordinal == cThis.ordinal + 1 then
        -- down, eg 3 to 2
        else
            return false, nil
        end
    end
    return true
end

function CheckTableauMovable(cPrev, cThis)
    -- One card can be moved at a time, but sequences can also be moved as one unit. 
    return CheckTableau(cPrev, cThis)
end

-- function CheckTableauTail(pileLen, tailLen)
--     return true, nil
-- end

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
    -- for i = 1, 4 do
    --     if PileCardCount(TABLEAUX[i]) == 0 then
    --         MoveCard(RESERVE, TABLEAUX[i])
    --     end
    -- end
end
