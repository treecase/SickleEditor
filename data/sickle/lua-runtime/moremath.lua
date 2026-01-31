
local moremath = {}

function moremath.clamp(x, min, max)
    if x < min then return min
    elseif x > max then return max
    else return x end
end

function moremath.round(x)
    if x < 0 then
        if math.ceil(x) - x < 0.5 then
            return math.ceil(x)
        end
        return math.floor(x)
    else
        if x - math.floor(x) < 0.5 then
            return math.floor(x)
        end
        return math.ceil(x)
    end
end

function moremath.round_to(x, multiple)
    return moremath.round(x / multiple) * multiple
end

return moremath
