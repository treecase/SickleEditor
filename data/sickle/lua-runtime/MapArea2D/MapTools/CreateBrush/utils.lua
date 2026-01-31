
local utils = {}

-- Find north-west and south-east corners of BOX. Points are in drawspace.
function utils.find_corners(box, maparea)
    local points = {box:get_start(), box:get_end()}
    local northwest = maparea:worldspace_to_drawspace(points[1])
    local southeast = maparea:worldspace_to_drawspace(points[1])
    for _,point in ipairs(points) do
        local point = maparea:worldspace_to_drawspace(point)
        if point.x < northwest.x then
            northwest.x = point.x
        end
        if point.y < northwest.y then
            northwest.y = point.y
        end
        if point.x > southeast.x then
            southeast.x = point.x
        end
        if point.y > southeast.y then
            southeast.y = point.y
        end
    end
    return northwest, southeast
end

-- Find north-west and south-east corners of BOX. Points are in drawspace3.
function utils.find_corners3(box, maparea)
    local points = {box:get_start(), box:get_end()}
    local northwest = maparea:worldspace_to_drawspace3(points[1])
    local southeast = maparea:worldspace_to_drawspace3(points[1])
    for _,point in ipairs(points) do
        local point = maparea:worldspace_to_drawspace3(point)
        if point.x < northwest.x then
            northwest.x = point.x
        end
        if point.y < northwest.y then
            northwest.y = point.y
        end
        if point.z < northwest.z then
            northwest.z = point.z
        end

        if point.x > southeast.x then
            southeast.x = point.x
        end
        if point.y > southeast.y then
            southeast.y = point.y
        end
        if point.z > southeast.z then
            southeast.z = point.z
        end
    end
    return northwest, southeast
end

return utils
