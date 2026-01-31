-- CreateBrush.MoveDrag
--
-- Move the BrushBox.

local utils = require "MapArea2D/MapTools/CreateBrush/utils"


local MoveDrag = {}
MoveDrag.metatable = {}
MoveDrag.metatable.__index = MoveDrag.metatable


function MoveDrag.metatable:on_button_press_event(event)
    return true
end

function MoveDrag.metatable:on_button_release_event(event)
    self.parent:removeListener(self)
    return self.moved
end

function MoveDrag.metatable:on_key_press_event(event)
end

function MoveDrag.metatable:on_key_release_event(event)
end

function MoveDrag.metatable:on_motion_notify_event(event)
    local snapped = (self.maparea.alt ~= true)
    local mousepos = self.maparea:screenspace_to_drawspace(
        geo.vec2.new(event.x, event.y))
    if snapped then
        mousepos = geo.vec2.map(round_to_grid, mousepos)
    end

    local delta = (
        self.maparea:drawspace_to_worldspace(mousepos)
        - self.maparea:drawspace_to_worldspace(self.origin))

    local brushbox = self.maparea:get_editor():get_brushbox()
    brushbox:set_start(self.bb_origin[1] + delta)
    brushbox:set_end(self.bb_origin[2] + delta)

    self.moved = true
    return true
end


function MoveDrag.new(parent, maparea, x, y)
    local click_pos = maparea:screenspace_to_drawspace(geo.vec2.new(x, y))
    local snapped = (maparea.alt ~= true)
    if snapped then
        click_pos = geo.vec2.map(round_to_grid, click_pos)
    end
    local brushbox = maparea:get_editor():get_brushbox()

    local northwest, southeast = utils.find_corners(brushbox, maparea)
    local corners = {
        northwest,
        geo.vec2.new(southeast.x, northwest.y),
        geo.vec2.new(northwest.x, southeast.y),
        southeast
    }
    local closest = {corners[1], math.maxinteger}
    for _,corner in ipairs(corners) do
        local distance = geo.vec2.length(corner - click_pos)
        if distance < closest[2] then
            closest = {corner, distance}
        end
    end

    local drag = {}
    drag.parent = parent
    drag.maparea = maparea
    drag.moved = false
    drag.origin = closest[1]
    drag.bb_origin = {brushbox:get_start(), brushbox:get_end()}
    setmetatable(drag, MoveDrag.metatable)
    return drag
end

return MoveDrag
