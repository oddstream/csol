-- Spider

PACKS = 8
SUITS = 1

-- PACKS = 4
-- SUITS = 2

-- PACKS = 2
-- SUITS = 4

POWERMOVES = false

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
    SetRecycles(STOCK, 0)

    TABLEAU = {}
  
    for x = 3, 10 do
        local pile = AddPile("Discard", x, 1, FAN_NONE)
        SetDraggable(pile, false)
    end

    for x = 1, 4 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN)
        DealDown(pile, 2)
        DealUp(pile, 1)
        TABLEAU[x] = pile
    end

    for x = 5, 10 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN)
        DealDown(pile, 1)
        DealUp(pile, 1)
        TABLEAU[x] = pile
    end
end

function CheckDiscardAccept(cThis)
  if cThis.ordinal == 13 then
    return true, nil
  else
    return false, "An empty Discard can only accept an King, not a " .. cThis.ordinal
  end
end

function CheckDiscard(cPrev, cThis)
  if cPrev.suit ~= cThis.suit then
    io.stderr:write("CheckDiscard suit fail\n")
    return false, nil
  end
  if cPrev.ordinal + 1 ~= cThis.ordinal then
    io.stderr:write("CheckDiscard ordinal fail\n")
    return false, nil
  end
  return true
end

function CheckDiscardMovable(cPrev, cThis)
  -- LogCard("CheckDiscardMovable prev", cPrev)
  -- LogCard("CheckDiscardMovable this", cThis)
  if cPrev.suit ~= cThis.suit then
    io.stderr:write("CheckDiscardMovable suit fail\n")
    return false, nil
  end
  if cPrev.ordinal + 1 ~= cThis.ordinal then
    io.stderr:write("CheckDiscardMovable ordinal fail\n")
    return false, nil
  end
  return true
end

function CheckTableauAccept(cThis)
  return true, nil
end

function CheckTableau(cPrev, cThis)
  if cPrev.ordinal ~= cThis.ordinal + 1 then
    io.stderr:write("CheckTableau ordinal fail\n")
    return false, nil
  end
  return true
end

function CheckTableauMovable(cPrev, cThis)
  -- LogCard("CheckTableauMovable prev", cPrev)
  -- LogCard("CheckTableauMovable this", cThis)
  if cPrev.suit ~= cThis.suit then
    io.stderr:write("CheckTableauMovable suit fail\n")
    return false, nil
  end
  if cPrev.ordinal ~= cThis.ordinal + 1 then
    io.stderr:write("CheckTableauMovable ordinal fail\n")
    return false, nil
  end
  return true
end

function CardTapped(card)
    LogCard("CardTapped", card)

    local cardsMoved = false
    local errMsg = nil
    local tabCards = 0
    local emptyTabs = 0
    if card.owner == STOCK then
      for _, tab in ipairs(TABLEAU) do
        if CardCount(tab) == 0 then
          emptyTabs = emptyTabs + 1
        else
          tabCards = tabCards + CardCount(tab)
        end
      end
      if emptyTabs > 0 and tabCards >= #TABLEAU then
        errMsg = "All empty tableaux must be filled before dealing a new row"
      else
        for _, tab in ipairs(TABLEAU) do
          DealUp(tab, 1)
          cardsMoved = true
        end
      end
    end

    return cardsMoved, errMsg
end
