-- Poker Shuffle

-- https://en.wikipedia.org/wiki/Poker_squares

dofile("variants/~Library.lua")

-- C sets variables 'BAIZE', FAN_*, and tables to hold pile functions and piles

COMPLETE = false

function BuildPiles()

    AddPile("Stock", -5, -5, FAN_NONE, 1, 4)
    
    AddPile("Reserve", 1, 1, FAN_DOWN)
    for n=1, 25 do
        local card = MoveCard(Stock.Pile, Reserve.Pile)
        CardProne(card, true)
    end
    CardProne(Last(Reserve.Pile), false)

    for x = 3, 7 do
        for y = 1, 5 do
            local pile = AddPile("Cell", x, y, FAN_NONE)
        end
    end

    XSCORES = {}
    for y = 1, 5 do
        local pile = AddPile("Label", 8, y, FAN_NONE)
        table.insert(XSCORES, pile)
        -- PileLabel(pile, "X" .. #XSCORES)
    end

    YSCORES = {}
    for x = 3, 7 do
        local pile = AddPile("Label", x, 6, FAN_NONE)
        table.insert(YSCORES, pile)
        -- PileLabel(pile, "Y" .. #YSCORES)
    end

    TOTAL = AddPile("Label", 8, 6, FAN_NONE)
    PileLabel(TOTAL, "Total")
end

function StartGame()
    COMPLETE = false
end

-- TailMoveError constraints (Tableau only)

-- function Cell.TailMoveError(tail)
--     if COMPLETE then
--         return "Game Complete"
--     end
--     return nil
-- end

-- TailAppendError constraints

-- function Cell.TailAppendError(pile, tail)
--     if COMPLETE then
--         return "Game Complete"
--     end
--     return nil
-- end

-- Actions

local function isRoyal(map)
    -- we know that arr is a straight AND a flush, but is it royal?
  
    -- KQJX98765432A
    -- 1111100000000 7936
    -- 1111000000001 7681 (special case)
  
    return map == 7936 or map == 7681
end
  
local function isFlush(arr)
    local s = CardSuit(arr[1])
    for i=2,5 do
        if CardSuit(arr[i]) ~= s then return false end
    end
    return true
end
  
local function isStraight(map)
    -- a straight need not be in order, so treat the cards as a set
  
    -- KQJX98765432A
    -- 0000000011111 31
    -- ...
    -- 0001111100000 992
    -- ...
    -- 1111100000000 7936
    -- 1111000000001 7681 (special case)
  
    if map == 7681 then return true end
  
    -- for (i=0;i<9;i++) { console.log(31<<i) }
    for _,test in ipairs({31,62,124,248,496,992,1984,3968,7936}) do
        if test == map then return true end
    end
  
    return false
end
  
local function isOfAKind(hist, n)
    for i=1,13 do
        if hist[i] == n then
            return true
        end
    end
    return false
end
  
local function numOf(hist, n)
    local count = 0
    for i=1,13 do
        if hist[i] == n then
            count = count + 1
        end
    end
    return count
end
  
local scoreTable = {
    ['Royal Flush']     = {uk=30, us=100},
    ['Straight Flush']  = {uk=30, us= 75},
    ['Quads']           = {uk=16, us= 50},
    ['Full House']      = {uk=10, us= 25},
    ['Flush']           = {uk= 5, us= 20},
    ['Straight']        = {uk=12, us= 15},
    ['Trips']           = {uk= 6, us= 10},
    ['Two Pair']        = {uk= 3, us=  5},
    ['One Pair']        = {uk= 1, us=  2},
}
  
local function calcScore(arr) -- an array of cards
    -- https://en.wikipedia.org/wiki/List_of_poker_hands
    if #arr ~= 5 then
        return nil
    end
  --[[
    local bits = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096}
    local histogram = {0,0,0,0,0,0,0,0,0,0,0,0,0}
    local map = 0
    for i=1,5 do
      local o = arr[i].ordinal
      map = bit.bor(map, bits[o])
      histogram[o] = histogram[o] + 1
    end
  ]]
    local histogram = {0,0,0,0,0,0,0,0,0,0,0,0,0}
    local bits = {false,false,false,false,false,false,false,false,false,false,false,false,false}
  
    -- now we have a modern Lua, could replace bits with bitwise ops on an integer
  
    for i=1,5 do
        local o = CardOrdinal(arr[i])
        histogram[o] = histogram[o] + 1
        bits[o] = true
    end
  
    local bitValues = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096}
    local map = 0
    for i=1,13 do
        if bits[i] then map = map + bitValues[i] end
    end
  
    local fl = isFlush(arr)
    local st = isStraight(map)
  
    if st and fl then
        if isRoyal(map) then return 'Royal Flush' else return 'Straight Flush' end
    elseif isOfAKind(histogram, 4) then return 'Quads'
    elseif isOfAKind(histogram, 3) and isOfAKind(histogram, 2) then return 'Full House'
    elseif fl then return 'Flush'
    elseif st then return 'Straight'
    elseif isOfAKind(histogram, 3) then return 'Trips'
    elseif numOf(histogram, 2) == 2 then return 'Two Pair'
    elseif isOfAKind(histogram, 2) then return 'One Pair'
    end
    return nil
end
  
local function getCard(x, y)
    -- Cell.Piles is an array of 25 piles in the order x=1 yyyyy x=2 yyyyy x=3 yyyyy x=4 yyyyy x=5 yyyyy
    local i = (x - 1) * 5 + y
    -- io.stdout:write(x .. "," .. y .. "=" .. i .. "\n")
    local pile = Cell.Piles[i]
    if pile then
        if Empty(Cell.Piles[i]) then
            return nil
        else
            return First(Cell.Piles[i])
        end
    else
        io.stderr:write("No pile at " .. x .. "," .. y .. "=" .. i .. "\n")
    end
end

function AfterMove()
--[[
    io.stdout:write("AfterMove\n")
    for y = 1, 5 do
        for x = 1, 5 do
            local c = getCard(x,y)
            if c then
                io.stdout:write(CardSuit(c) .. " ")
            else
                io.stdout:write("- ")
            end
        end
        io.stdout:write("\n")
    end
]]

    local total = 0

    for y = 1, 5 do
        local xarr = {}
        for x = 1, 5 do
            local c = getCard(x, y)
            if c then 
                table.insert(xarr, c)
            end
        end
        local result = calcScore(xarr)
        if result then
            -- io.stdout:write("X" .. "=" .. y .. " " .. result .. "\n")
            PileLabel(XSCORES[y], result)
            total = total + scoreTable[result].uk
        else
            PileLabel(XSCORES[y], "")
        end
    end

    for x = 1, 5 do
        local yarr = {}
        for y = 1, 5 do
            local c = getCard(x, y)
            if c then 
                table.insert(yarr, c)
            end
        end
        local result = calcScore(yarr)
        if result then
            -- io.stdout:write("Y" .. "=".. x .." " .. scoreTable[result].uk .. "\n")
            PileLabel(YSCORES[x], result)
            total = total + scoreTable[result].uk
        else
            PileLabel(YSCORES[x], "")
        end
    end

    PileLabel(TOTAL, total)

    if Empty(Reserve.Pile) then
        Toast("Score " .. total)
        COMPLETE = true
    end
end

function PercentComplete()
    return 100 - Len(Reserve.Pile) * 4
end
