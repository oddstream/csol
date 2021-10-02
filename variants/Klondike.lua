-- Klondike

PACKS = 1

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
    SetRecycles(STOCK, 9999)
  
    WASTE = AddPile("Waste", 2, 1, FAN_RIGHT3)
    
    local pile

    for x = 4, 7 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        SetAccept(pile, 1)
    end

    pile = AddPile("Tableau", 1, 2, FAN_DOWN)
    SetAccept(pile, 13)
    DealUp(pile, 1)

    pile = AddPile("Tableau", 2, 2, FAN_DOWN)
    SetAccept(pile, 13)
    DealDown(pile, 1)
    DealUp(pile, 1)

    pile = AddPile("Tableau", 3, 2, FAN_DOWN)
    SetAccept(pile, 13)
    DealDown(pile, 2)
    DealUp(pile, 1)

    pile = AddPile("Tableau", 4, 2, FAN_DOWN)
    SetAccept(pile, 13)
    DealDown(pile, 3)
    DealUp(pile, 1)

    pile = AddPile("Tableau", 5, 2, FAN_DOWN)
    SetAccept(pile, 13)
    DealDown(pile, 4)
    DealUp(pile, 1)

    pile = AddPile("Tableau", 6, 2, FAN_DOWN)
    SetAccept(pile, 13)
    DealDown(pile, 5)
    DealUp(pile, 1)

    pile = AddPile("Tableau", 7, 2, FAN_DOWN)
    SetAccept(pile, 13)
    DealDown(pile, 6)
    DealUp(pile, 1)

end

function CheckFoundationAccept(cThis)
  if cThis.ordinal == 1 then
    return true, nil
  else
    return false, "A Foundation can only accept an Ace, not a " .. cThis.ordinal
  end
end

function CheckFoundation(cPrev, cThis)
  if cPrev.prone or cThis.prone then
    io.stderr:write("CheckFoundation prone fail\n")
    return false, "Cannot move a face down card"
  end
  if cPrev.suit ~= cThis.suit then
    io.stderr:write("CheckFoundation suit fail\n")
    return false, nil
  end
  if cPrev.ordinal + 1 ~= cThis.ordinal then
    io.stderr:write("CheckFoundation ordinal fail\n")
    return false, nil
  end
  return true
end

--[[
function ConformantF(pile, cards)

  LogTail("ConformantF tail", cards)

  if not pile then
    io.stderr:write("ConformantF passed a nil pile\n")
  end

  if #cards == 0 then
    io.stderr:write("ConformantF passed an empty tail\n")
    return false
  end

  local ok, err

  if pile then
    local cTop = PeekCard(pile)
    if cTop then
      ok, err = TwoCardsF(cTop, cards[1])
      if not ok then
        return false, err
      end
    end
  end

  local cPrev = cards[1]
  for n=2, #cards do
    local cThis = cards[n]
    ok, err = TwoCardsF(cPrev, cThis)
    if not ok then
      return false, err
    end
    cPrev = cThis
  end

  return true, nil
end
]]

function CheckTableauAccept(cThis)
  if cThis.ordinal == 13 then
    return true, nil
  else
    return false, "A Tableau can only accept a King, not a " .. cThis.ordinal
  end
end

function CheckTableau(cPrev, cThis)
  if cPrev.prone or cThis.prone then
    io.stderr:write("CheckTableau prone fail\n")
    return false, "Cannot move a face down card"
  end
  if cPrev.color == cThis.color then
    io.stderr:write("CheckTableau color fail\n")
    return false, nil
  end
  if cPrev.ordinal ~= cThis.ordinal + 1 then
    io.stderr:write("CheckTableau ordinal fail\n")
    return false, nil
  end
  if PileCategory(cThis.owner) == "Foundation" then
    return false, "Cannot move a card from a Foundation to a Tableau"
  end
  return true
end

--[[
function ConformantT(pile, cards)

    LogTail("ConformantT tail", cards)
    
    local ok, err
  
    if pile then
      local cTop = PeekCard(pile)
      if cTop then
        ok, err = TwoCardsT(cTop, cards[1])
        if not ok then
          return false, err
        end
      end
    end

    local cPrev = cards[1]
    for n=2, #cards do
      local cThis = cards[n]
      ok, err = TwoCardsT(cPrev, cThis)
      if not ok then
        return false, err
      end
      cPrev = cThis
    end

    return true, nil
end
]]

function CardTapped(card)
  LogCard("CardTapped", card)

  local cardsMoved = false

  if card.owner == STOCK then
    cardsMoved = MoveCard(STOCK, WASTE)
  end

  return cardsMoved, nil
end
