-- Acme

--[[
  https://en.wikipedia.org/wiki/Acme_(solitaire)

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

dofile("variants/~Library.lua")

function Wikipedia()
    return "https://en.wikipedia.org/wiki/Acme_(solitaire)"
end

function BuildPiles()

    AddPile("Stock", 1, 1, FAN_NONE, 1, 4)
    AddPile("Waste", 2, 1, FAN_RIGHT3)

    for x = 4, 7 do
        local pile = AddPile("Foundation", x, 1, FAN_NONE)
        PileLabel(pile, U[1])
    end

    AddPile("Reserve", 1, 2, FAN_DOWN)

    for x = 4, 7 do
        AddPile("Tableau", x, 2, FAN_DOWN, MOVE_ONE)
    end

end

function StartGame()

    StockRecycles(1)

    local pile = Reserve.Pile
    for n = 1, 13 do
        local c = MoveCard(Stock.Pile, pile)
        CardProne(c, true)
    end
    CardProne(Last(pile), false)

    for _, pile in ipairs(Tableau.Piles) do
        MoveCard(Stock.Pile, pile)
    end

end

-- TailMoveError constraints (Tableau only)

function Tableau.TailMoveError(tail)
    if Len(tail) > 1 then
        local c1 = Get(tail, 1)
        for i = 2, Len(tail) do
            local c2 = Get(tail, i)
            local err = DownSuit(c1, c2) if err then return err end
            c1 = c2
        end
    end
    return nil
end

-- TailAppendError constraints

function Foundation.TailAppendError(pile, tail)
    if Len(pile) == 0 then
        local c1 = First(tail)
        if CardOrdinal(c1) ~= 1 then
            return "Foundation can only accept an Ace, not a " .. V[CardOrdinal(c1)]
        end
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = UpSuit(c1, c2) if err then return err end
    end
    return nil
end

function Tableau.TailAppendError(pile, tail)
    if Len(pile) == 0 then
        -- do nothing, empty accept any card
    else
        local c1 = Last(pile)
        local c2 = First(tail)
        local err = DownSuit(c1, c2) if err then return err end
    end
    return nil
end

function Tableau.UnsortedPairs(pile)
    local unsorted = 0
    local c1 = Get(pile, 1)
    for i = 2, Len(pile) do
        local c2 = Get(pile, i)
        local err = DownSuit(c1, c2)
        if err then
            unsorted = unsorted + 1
        end
        c1 = c2
    end
    return unsorted
end

-- Actions

function Stock.PileTapped(pile)
    recycles = StockRecycles()
    if recycles == 0 then
        return "No more Stock recycles"
    elseif 1 == recycles then
        Toast("Last Stock recycle")
    elseif 2 == recycles then
        Toast("One Stock recycle remaining")
    end
    if Len(Waste.Pile) > 0 then
        while Len(Waste.Pile) > 0 do
            MoveCard(Waste.Pile, Stock.Pile)
        end
        recycles = recycles - 1
        StockRecycles(recycles)
    end
end

function Stock.TailTapped(tail)
    MoveCard(Stock.Pile, Waste.Pile)
end

function AfterMove()
    for i = 1, 4 do
        if Len(Tableau.Piles[i]) == 0 then
            MoveCard(Reserve.Pile, Tableau.Piles[i])
        end
    end
end
