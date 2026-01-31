-- SelectionDrag
--
-- Move the selected brushes.
-- Snaps to the bounding-box corner closest to where the user clicked to start
-- the drag.


-- Calculate the top-left and bottom-right corners of the given brushes.
local function find_corners(brushes)
    local topleft = {nil, nil, 0}
    local bottomright = {nil, nil, 0}
    for brush in brushes do
        for i,vertex in ipairs(brush:get_vertices()) do
            if topleft[1] == nil or vertex.x < topleft[1] then
                topleft[1] = vertex.x
            end
            if bottomright[1] == nil or vertex.x > bottomright[1] then
                bottomright[1] = vertex.x
            end
            if topleft[2] == nil or vertex.y > topleft[2] then
                topleft[2] = vertex.y
            end
            if bottomright[2] == nil or vertex.y < bottomright[2] then
                bottomright[2] = vertex.y
            end
        end
    end
    return geo.vec3.new(topleft), geo.vec3.new(bottomright)
end


local MoveSelected = {}
MoveSelected.metatable = {}
MoveSelected.metatable.__index = MoveSelected.metatable


function MoveSelected.metatable:snapped()
    return self.maparea.alt == false
end


function MoveSelected.metatable:on_button_press_event(event)
    return true
end

function MoveSelected.metatable:on_button_release_event(event)
    self.parent:removeListener(self)
    return self.moved
end

function MoveSelected.metatable:on_key_press_event(event)
end

function MoveSelected.metatable:on_key_release_event(event)
end

function MoveSelected.metatable:on_motion_notify_event(event)
    local curr = self.maparea:screenspace_to_drawspace(
        geo.vec2.new(event.x, event.y))

    local newcorner = curr
    if self:snapped() then
        newcorner = geo.vec2.map(round_to_grid, curr)
    end

    local translation = (
        self.maparea:drawspace_to_worldspace(newcorner)
        - self.maparea:drawspace_to_worldspace(self.corner)
    )
    for brush in self.maparea:get_editor():get_selection():iterate() do
        brush:translate(translation - self.prev_translation)
    end
    self.prev_translation = translation

    self.moved = true
    return true
end

function MoveSelected.metatable:on_scroll_event(event)
end


function MoveSelected.new(parent, maparea, x, y)
    local click_pos = maparea:screenspace_to_drawspace(geo.vec2.new(x, y))

    local topleft, bottomright = find_corners(
        maparea:get_editor():get_selection():iterate())

    topleft = maparea:worldspace_to_drawspace(topleft)
    bottomright = maparea:worldspace_to_drawspace(bottomright)

    local corners = {
        topleft,
        geo.vec2.new(bottomright.x, topleft.y),
        geo.vec2.new(topleft.x, bottomright.y),
        bottomright
    }

    local closest_corner = nil
    for i,corner in ipairs(corners) do
        local distance = geo.vec2.length(corner - click_pos)
        if closest_corner == nil or distance < closest_corner[2] then
            closest_corner = {corner, distance}
        end
    end

    local drag = {}
    drag.parent = parent
    drag.maparea = maparea
    drag.moved = false
    drag.corner = closest_corner[1]
    drag.prev_translation = geo.vec3.new()
    setmetatable(drag, MoveSelected.metatable)
    return drag
end

return MoveSelected
