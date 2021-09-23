-- Spider

-- PACKS = 8
-- SUITS = 1

PACKS = 4
SUITS = 2

-- C sets variable 'BAIZE', 'STOCK', FAN_*

function LogCard(title, card)
  if card then
    io.stderr:write(title .. " {ordinal:" .. card.ordinal .. " suit:" .. card.suit .. " color:" .. card.color .. " owner:" .. Category(card.owner) .. "}\n")
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

    -- local s = FindPile("Stock", 1)
    -- if s == nil then
    --     io.stderr:write("Build cannot find Stock pile\n")
    --     return
    -- else
    --     io.stderr:write("Build found a pile\n")
    --     MovePileTo(s, 10, 100)
    -- end

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

function BuildFoundation(cTop, cards)

  -- ignore cTop

  if #cards == 0 then
    io.stderr:write("BuildFoundation passed an empty tail\n")
    return false
  end

  if #cards ~= 13 then
      io.stderr:write("BuildFoundation tail needs to be 13 cards long\n")
      return false
  end

  if cards[1].ordinal ~= 13 then
    io.stderr:write("BuildFoundation tail needs to start with a K\n")
    return false
  end
  
  local cPrev = cards[1]
  for n=2, #cards do
    local cThis = cards[n]
    if cPrev.prone or cThis.prone then
      io.stderr:write("BuildFoundation prone fail\n")
      return false
    end
    if cPrev.suit ~= cThis.suit then
      io.stderr:write("BuildFoundation suit fail\n")
      return false
    end
    if cPrev.ordinal ~= cThis.ordinal + 1 then
      io.stderr:write("BuildFoundation ordinal fail\n")
      return false
    end
    cPrev = cThis
  end

  return true
end

function ChkTBuild(cPrev, cThis)
  if cPrev.prone or cThis.prone then
    io.stderr:write("ChkT prone fail\n")
    return false
  end
  if cPrev.ordinal ~= cThis.ordinal + 1 then
    io.stderr:write("ChkT ordinal fail\n")
    return false
  end
  return true
end

function ChkTDrag(cPrev, cThis)
  if cPrev.prone or cThis.prone then
    io.stderr:write("ChkT prone fail\n")
    return false
  end
  if cPrev.suit ~= cThis.suit then
    io.stderr:write("ChkT suit fail\n")
    return false
  end
  if cPrev.ordinal ~= cThis.ordinal + 1 then
    io.stderr:write("ChkT ordinal fail\n")
    return false
  end
  return true
end

function BuildTableau(cTop, cards)

  LogCard("BuildTableau card", cTop)
  LogTail("BuildTableau tail", cards)
  
  if cTop then
    if not ChkTBuild(cTop, cards[1]) then
      return false
    end
  end

  local cPrev = cards[1]
  for n=2, #cards do
    local cThis = cards[n]
    if not ChkTBuild(cPrev, cThis) then
      return false
    end
    cPrev = cThis
  end

  return true
end

function DragTableau(cTop, cards)
  -- ignore cTop
  local cPrev = cards[1]
  for n=2, #cards do
    local cThis = cards[n]
    if not ChkTDrag(cPrev, cThis) then
      return false
    end
    cPrev = cThis
  end
  return true
end

function ChkFalse(cTop, cards)
  LogCard("ChkFalse card", cTop)
  LogTail("ChkFalse tail", cards)
  return false
end

function ChkTrue(cTop, cards)
  LogCard("ChkTrue card", cTop)
  LogTail("ChkTrue tail", cards)
  return true
end

function CardTapped(card)
    LogCard("CardTapped", card)

    local cardsMoved = false
    if card.owner == STOCK then
      for _, tab in ipairs(TABLEAU) do
        DealUp(tab, 1)
        cardsMoved = true
      end
    end

    return cardsMoved, ""
end