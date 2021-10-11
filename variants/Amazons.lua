-- Amazons

--[[
    https://en.wikipedia.org/wiki/Amazons_(solitaire)
    Morehead and Mott-Smith, p179
]]

PACKS = 1
SUITS = 4
POWERMOVES = false
STRIP_CARDS = {2,3,4,5,6,13}

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

    -- a stock pile is always created first, and filled with Packs of shuffled cards
    PileMoveTo(STOCK, 6, 1)

    FOUNDATIONS = {}
    for x = 1, 4 do
        local pile = AddPile("Foundation", x, 1, FAN_NONE)
        FOUNDATIONS[#FOUNDATIONS+1] = pile
        SetPileAccept(pile, 1)
    end

    RESERVES = {}
    for x = 1, 4 do
        pile = AddPile("Reserve", x, 2, FAN_DOWN)
        RESERVES[#RESERVES+1] = pile
        MoveCard(STOCK, pile)
    end

end

function StartGame()
    -- STOCK_RECYCLES = 999
    -- SetPileRecycles(STOCK, STOCK_RECYCLES)
end

function CalcPileIndex(piles, pile)
    for i, p in ipairs(piles) do
        if p == pile then
            return i
        end
    end
    io.stderr:write("CalcPileIndex cannot find pile\n")
end

function FoundationAccept(pile, cThis)
    if cThis.ordinal ~= 1 then
        return false, "An empty Foundation can only accept an Ace, not a " .. cThis.ordinal
    end
    local itarget = CalcPileIndex(FOUNDATIONS, pile)
    local isource = CalcPileIndex(RESERVES, cThis.owner)
    if isource ~= itarget then
        return false, "Aces can only be placed on the Foundation above"
    end
    return true
end

function FoundationBuildPair(cPrev, cThis)
    -- work out the index of the target pile
    if cThis.ordinal ~= 12 then
        local itarget = CalcPileIndex(FOUNDATIONS, cPrev.owner)
        local isource = CalcPileIndex(RESERVES, cThis.owner)
        if isource ~= itarget then
            return false, "Cards can only be placed on the Foundation above"
        end
    end
    if cPrev.suit ~= cThis.suit then
        -- io.stderr:write("CheckFoundation suit fail\n")
        return false, nil
    end
    if cPrev.ordinal == 1 and cThis.ordinal == 7 then
        return true 
    elseif cPrev.ordinal + 1 == cThis.ordinal then
        return true
    else
        return false, nil
    end
    return true
end

function CardTapped(card)
    -- LogCard("CardTapped", card)
    if card.owner == STOCK then
        for _, res in ipairs(RESERVES) do
            MoveCard(STOCK, res)
        end
    end
end

function PileTapped(pile)
    -- io.stdout:write("PileTapped\n")
    if pile == STOCK then
        for _, res in ipairs(RESERVES) do
            MoveAllCards(res, STOCK)
            -- while PileCardCount(res) > 0 do
            --     MoveCard(res, STOCK)
            -- end
        end
    end
    return nil  -- no error striing
end

function AfterMove()
    -- io.stdout:write("AfterMove\n")
end
