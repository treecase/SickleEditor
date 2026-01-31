-- CreateBrush.Create
--
-- Create the brush box.


local Create = {}
Create.metatable = {}
Create.metatable.__index = Create.metatable


function Create.metatable:on_button_press_event(event)
    return true
end

function Create.metatable:on_button_release_event(event)
    self.parent:removeListener(self)
    return self.moved
end

function Create.metatable:on_key_press_event(event)
end

function Create.metatable:on_key_release_event(event)
end

function Create.metatable:on_motion_notify_event(event)
    local snapped = (self.maparea.alt ~= true)

    -- Selection is cleared here since this drag can be triggered by a simple
    -- click, too.
    self.maparea:get_editor():get_selection():clear()

    local curr = self.maparea:screenspace_to_drawspace(
        geo.vec2.new(event.x, event.y))
    if snapped then
        curr = geo.vec2.map(round_to_grid, curr)
    end

    local brushbox = self.maparea:get_editor():get_brushbox()
    if not self.moved then
        brushbox:set_start(self.maparea:drawspace_to_worldspace(curr))
    end
    local curr3 = geo.vec3.new(curr.x, curr.y, gAppWin:get_grid_size())
    brushbox:set_end(self.maparea:drawspace3_to_worldspace(curr3))

    self.moved = true
    return true
end

function Create.metatable:on_scroll_event(event)
end


function Create.new(parent, maparea, x, y)
    local brushbox = maparea:get_editor():get_brushbox()
    brushbox:set_start(geo.vec3.new())
    brushbox:set_end(geo.vec3.new())

    local drag = {}
    drag.parent = parent
    drag.maparea = maparea
    drag.moved = false
    setmetatable(drag, Create.metatable)
    return drag
end

return Create
