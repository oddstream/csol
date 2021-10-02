-- Spider

PACKS = 8
SUITS = 1

-- PACKS = 4
-- SUITS = 2

-- PACKS = 2
-- SUITS = 4

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
        local pile = AddPile("Foundation", x, 1, FAN_NONE, "BuildFoundation", "ChkFalse")
    end

    for x = 1, 4 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN, "BuildTableau", "DragTableau")
        DealDown(pile, 5)
        DealUp(pile, 1)
        TABLEAU[x] = pile
    end

    for x = 5, 10 do
        local pile = AddPile("Tableau", x, 2, FAN_DOWN, "BuildTableau", "DragTableau")
        DealDown(pile, 4)
        DealUp(pile, 1)
        TABLEAU[x] = pile
    end
end

function BuildFoundation(pile, cards)

  -- ignore cTop

  if #cards == 0 then
    io.stderr:write("BuildFoundation passed an empty tail\n")
    return false
  end

  if #cards ~= 13 then
      io.stderr:write("BuildFoundation tail needs to be 13 cards long\n")
      return false, "Needs to be 13 cards"
  end

  if cards[1].ordinal ~= 13 then
    io.stderr:write("BuildFoundation tail needs to start with a K\n")
    return false, "Needs to start with a King"
  end
  
  local ok, err

  local cPrev = cards[1]
  for n=2, #cards do
    local cThis = cards[n]
    if cPrev.prone or cThis.prone then
      io.stderr:write("BuildFoundation prone fail\n")
      return false, "Cannot move a face down card"
    end
    if cPrev.suit ~= cThis.suit then
      io.stderr:write("BuildFoundation suit fail\n")
      return false, nil
    end
    if cPrev.ordinal ~= cThis.ordinal + 1 then
      io.stderr:write("BuildFoundation ordinal fail\n")
      return false, nil
    end
    cPrev = cThis
  end

  return true
end

function TwoCardsBuildT(cPrev, cThis)
  if cPrev.prone or cThis.prone then
    io.stderr:write("TwoCardsBuildT prone fail\n")
    return false, "Cannot move a face down card"
  end
  if cPrev.ordinal ~= cThis.ordinal + 1 then
    io.stderr:write("TwoCardsBuildT ordinal fail\n")
    return false, nil
  end
  return true
end

function TwoCardsDragT(cPrev, cThis)
  if cPrev.prone or cThis.prone then
    io.stderr:write("TwoCardsDragT prone fail\n")
    return false, "Cannot move a face down card"
  end
  if cPrev.suit ~= cThis.suit then
    io.stderr:write("TwoCardsDragT suit fail\n")
    return false, nil
  end
  if cPrev.ordinal ~= cThis.ordinal + 1 then
    io.stderr:write("TwoCardsDragT ordinal fail\n")
    return false, nil
  end
  return true
end

function BuildTableau(pile, cards)

  LogTail("BuildTableau tail", cards)
  
  local ok, err

  if pile then
    local cTop = PeekCard(pile)
    if cTop then
      ok, err = TwoCardsBuildT(cTop, cards[1])
      if not ok then
        return false, err
      end
    end
  end

  local cPrev = cards[1]
  for n=2, #cards do
    local cThis = cards[n]
    ok, err = TwoCardsBuildT(cPrev, cThis)
    if not ok then
      return false, err
    end
    cPrev = cThis
  end

  return true
end

function DragTableau(pile, cards)
  local ok, err
  local cPrev = cards[1]
  for n=2, #cards do
    local cThis = cards[n]
    ok, err = TwoCardsDragT(cPrev, cThis)
    if not ok then
      return false, err
    end
    cPrev = cThis
  end
  return true
end

function ChkFalse(pile, cards)
  LogTail("ChkFalse tail", cards)
  return false, "You cannot do that"
  end

function ChkTrue(pile, cards)
  LogTail("ChkTrue tail", cards)
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
