-- Spider

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

    -- a stock pile is always created first, and filled with Packs of shuffled cards
    PileMoveTo(STOCK, 1, 1)
    SetPileRecycles(STOCK, 0)

    TABLEAU = {}
  
    for x = 3, 10 do
        local pile = AddPile("Discard", x, 1, FAN_NONE)
    end

    for x = 1, 4 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN)
        for n=1,4 do
          local c = MoveCard(STOCK, pile)
          SetCardProne(c, true)
        end
        MoveCard(STOCK, pile)
        TABLEAU[x] = pile
    end

    for x = 5, 10 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN)
        for n=1,3 do
          local c = MoveCard(STOCK, pile)
          SetCardProne(c, true)
        end
        MoveCard(STOCK, pile)
        TABLEAU[x] = pile
    end
end

function DiscardAccept(pile, cThis)
    if PileType(cThis.owner) == "Discard" then
        return false, "Cannot move cards from a Discard"
    end
    if cThis.ordinal ~= 13 then
      return false, "An empty Discard can only accept an King, not a " .. cThis.ordinal
    end
    return true
end

function DiscardBuildPair(cPrev, cThis)
    if PileType(cThis.owner) == "Discard" then
        return false, "Cannot move cards from a Discard"
    end
    if cPrev.suit ~= cThis.suit then
        -- io.stderr:write("CheckDiscard suit fail\n")
        return false, nil
    end
    if cPrev.ordinal + 1 ~= cThis.ordinal then
        -- io.stderr:write("CheckDiscard ordinal fail\n")
        return false, nil
    end
    return true
end

function DiscardMovePair(cPrev, cThis)
    if PileType(cThis.owner) == "Discard" then
        return false, "Cannot move cards from a Discard"
    end
    -- LogCard("CheckDiscardMovable prev", cPrev)
    -- LogCard("CheckDiscardMovable this", cThis)
    if cPrev.suit ~= cThis.suit then
      -- io.stderr:write("CheckDiscardMovable suit fail\n")
      return false, nil
    end
    if cPrev.ordinal + 1 ~= cThis.ordinal then
      -- io.stderr:write("CheckDiscardMovable ordinal fail\n")
      return false, nil
    end
    return true
end

function TableauAccept(pile, cThis)
    if PileType(cThis.owner) == "Discard" then
        return false, "Cannot move cards from a Discard"
    end
    return true
end

function TableauBuildPair(cPrev, cThis)
    if PileType(cThis.owner) == "Discard" then
        return false, "Cannot move cards from a Discard"
    end
    if cPrev.ordinal ~= cThis.ordinal + 1 then
        -- io.stderr:write("CheckTableau ordinal fail\n")
        return false, nil
    end
    return true
end

function TableauMovePair(cPrev, cThis)
    if PileType(cThis.owner) == "Discard" then
        return false, "Cannot move cards from a Discard"
    end
    -- LogCard("CheckTableauMovable prev", cPrev)
    -- LogCard("CheckTableauMovable this", cThis)
    if cPrev.suit ~= cThis.suit then
      -- io.stderr:write("CheckTableauMovable suit fail\n")
      return false, nil
    end
    if cPrev.ordinal ~= cThis.ordinal + 1 then
        -- io.stderr:write("CheckTableauMovable ordinal fail\n")
      return false, nil
    end
    return true
end

function CardTapped(card)
    -- LogCard("CardTapped", card)
    local errMsg = nil
    local tabCards = 0
    local emptyTabs = 0
    if card.owner == STOCK then
      for _, tab in ipairs(TABLEAU) do
        if PileCardCount(tab) == 0 then
          emptyTabs = emptyTabs + 1
        else
          tabCards = tabCards + PileCardCount(tab)
        end
      end
      if emptyTabs > 0 and tabCards >= #TABLEAU then
        errMsg = "All empty tableaux must be filled before dealing a new row"
      else
        for _, tab in ipairs(TABLEAU) do
          MoveCard(STOCK, tab)
        end
      end
    end

    return errMsg
end
