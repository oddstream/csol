-- American Toad

--[[
American Toad is a solitaire game using two decks of playing cards.
This game is similar to Canfield except that the tableau builds down in suit, and a partial tableau stack cannot be moved (only the top card or entire stack can be moved).
The object of the game is to move all cards to the foundations.
American Toad has eight tableau stacks. Each tableau stack contains one card and builds down in suit wrapping from Ace to King, e.g. 3♠, 2♠, A♠, K♠...
There are also eight foundations that build up in suit, e.g. 7♦, 8♦, 9♦...
The game includes one reserve pile with twenty cards that can be played onto the tableau or foundations.
There is a deck usually at the bottom right that turns up one card at a time.
One card is dealt onto the first foundation. This rank will be used as a base for the other foundations.
The foundations build up in suit, wrapping from King to Ace as necessary. 
]]

PACKS = 2
SUITS = 4
POWERMOVES = false

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

    -- a stock pile is always created first, and filled with PACKS of shuffled cards
    PileMoveTo(STOCK, 1, 1)

    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)

    RESERVE = AddPile("Reserve", 4, 1, FAN_RIGHT)
    for n = 1, 20 do
        local c = MoveCard(STOCK, RESERVE)
        SetCardProne(c, true)
    end
    SetCardProne(PilePeekCard(RESERVE), false)
    
    FOUNDATIONS = {}
    for x = 1, 8 do
        local pile = AddPile("Foundation", x, 2, FAN_NONE)
        SetPileDraggable(pile, false)
        -- FOUNDATIONS[#FOUNDATIONS+1] = pile
        table.insert(FOUNDATIONS, pile)
    end

    TABLEAUX = {}
    for x = 1, 8 do
        local pile = AddPile("Tableau", x, 3, FAN_DOWN)
        MoveCard(STOCK, pile)
        table.insert(TABLEAUX, pile)
    end

end

function StartGame()
    STOCK_RECYCLES = 1
    SetPileRecycles(STOCK, STOCK_RECYCLES)
    MoveCard(STOCK, FOUNDATIONS[1])
    local c = PilePeekCard(FOUNDATIONS[1])
    local t = CardToTable(c)
    FOUNDATION_ACCEPT = t.ordinal
    -- LogCard("Setting Accept", t)
    for i = 1, 8 do
        SetPileAccept(FOUNDATIONS[i], FOUNDATION_ACCEPT)
    end
    MoveCard(STOCK, WASTE)
end

function CheckFoundation(cPrev, cThis)
    if not cPrev then
        if cThis.ordinal ~= FOUNDATION_ACCEPT then
            return false, "An empty Foundation can only accept a " .. FOUNDATION_ACCEPT .. " not a " .. cThis.ordinal
        end
    else
        -- The foundations build up in suit, wrapping from King to Ace as necessary. 
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
        -- accept any card onto an empty pile
    else
        -- Each tableau stack contains one card and builds down in suit wrapping from Ace to King, e.g. 3♠, 2♠, A♠, K♠...
        if cPrev.suit ~= cThis.suit then
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
    -- can only move a single card or a whole pile
    return CheckTableau(cPrev, cThis)
end

function CheckTableauTail(pileLen, tailLen)
    io.stderr:write("CheckTableauTail(" .. pileLen .. "," .. tailLen .. ")\n")

    if tailLen == 1 or pileLen == tailLen then
        return true, nil
    end
    return false, "Can only move one card, or the whole pile"
end

function CardTapped(card)
--   LogCard("CardTapped", card)
  if card.owner == STOCK then
    MoveCard(STOCK, WASTE)
  end
end

function PileTapped(pile)
--   io.stdout:write("PileTapped\n")
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
