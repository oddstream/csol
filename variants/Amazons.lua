-- Amazons

--[[
    https://en.wikipedia.org/wiki/Amazons_(solitaire)
    Morehead and Mott-Smith, p179
]]

V = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"}
POWERMOVES = false

function BuildPiles()

    STOCK = AddPile("Stock", 6, 1, FAN_NONE, 1, 4, {2,3,4,5,6,13})

    local pile

    FOUNDATIONS = {}
    for x = 1, 4 do
        pile = AddPile("Foundation", x, 1, FAN_NONE)
        table.insert(FOUNDATIONS, pile)
        SetPileAccept(pile, 1)
    end

    RESERVES = {}
    for x = 1, 4 do
        pile = AddPile("Reserve", x, 2, FAN_DOWN)
        table.insert(RESERVES, pile)
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

-- CanTailBeMoved constraints (_Tableau only)

--[[
function CanTailBeMoved_Waste(tail)
    if TailLen(tail) > 1 then
        return false, "Only a single card can be moved from the Waste"
    end
    return true
end

function CanTailBeMoved_Foundation(tail)
    return false, "Cannot move cards from a Foundation"
end

function CanTailBeMoved_Reserve(tail)
    if TailLen(tail) > 1 then
        return false, "Only one Reserve card can be moved"
    end
    return true
end
]]

-- CanTailBeAppended constraints

function CanTailBeAppended_Waste(pile, tail)
    if CardOwner(TailGet(tail, 1)) ~= STOCK then
        return false, "The Waste can only accept cards from the Stock"
    end
    return true
end

function CanTailBeAppended_Foundation(pile, tail)
    if TailLen(tail) > 1 then
        return false, "Foundation can only accept a single card"
    elseif PileLen(pile) == 0 then
        local c1 = TailGet(tail, 1)
        if CardOrdinal(c1) ~= 1 then
            return false, "An empty Foundation can only accept an Ace, not a " .. V[CardOrdinal(c1)]
        end
        local itarget = CalcPileIndex(FOUNDATIONS, pile)
        local isource = CalcPileIndex(RESERVES, CardOwner(c1))
        if isource ~= itarget then
            return false, "Aces can only be placed on the Foundation above"
        end
    else
        local c1 = PilePeek(pile)
        local c2 = TailGet(tail, 1)
        -- work out the index of the target pile
        if CardOrdinal(c2) ~= 12 then
            local itarget = CalcPileIndex(FOUNDATIONS, CardOwner(c1))
            local isource = CalcPileIndex(RESERVES, CardOwner(c2))
            if isource ~= itarget then
                return false, "Cards can only be placed on the Foundation above"
            end
        end
        if CardSuit(c1) ~= CardSuit(c2) then
            return false, "Foundations build in suit"
        end
        if CardOrdinal(c1) == 1 and CardOrdinal(c2) == 7 then
            return true
        elseif CardOrdinal(c1) + 1 == CardOrdinal(c2) then
            return true
        else
            return false, "Foundations build up"
        end
    end
    return true
end

-- Actions

function CardTapped(card)
    if CardOwner(card) == STOCK then
        for _, res in ipairs(RESERVES) do
            MoveCard(STOCK, res)
        end
    end
end

function PileTapped(pile)
    if pile == STOCK then
        for _, res in ipairs(RESERVES) do
            MoveAllCards(res, STOCK)
            -- while PileCardCount(res) > 0 do
            --     MoveCard(res, STOCK)
            -- end
        end
    end
end

function AfterMove()
end
