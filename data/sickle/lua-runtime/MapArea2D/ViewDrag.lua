-- ViewDrag
--
-- Pans the view around.


local ViewDrag = {}
ViewDrag.metatable = {}
ViewDrag.metatable.__index = ViewDrag.metatable


function ViewDrag.metatable:on_button_press_event(event)
    return true
end

function ViewDrag.metatable:on_button_release_event(event)
    if event.button == 2 then
        self.maparea:removeListener(self)
        return self.moved
    end
end

function ViewDrag.metatable:on_key_press_event(event)
end

function ViewDrag.metatable:on_key_release_event(event)
end

function ViewDrag.metatable:on_motion_notify_event(event)
    assert(event.state & LuaGDK.GDK_BUTTON2_MASK ~= 0)

    local transform_xy = geo.vec2.new(
        self.maparea:get_transform():get_x(),
        self.maparea:get_transform():get_y())

    local mouse_position = (
        self.maparea:screenspace_to_drawspace(geo.vec2.new(event))
        + transform_xy)
    local delta = mouse_position - self.last_mouse_position

    local transform = self.maparea:get_transform()
    transform:set_x(transform:get_x() + delta.x)
    transform:set_y(transform:get_y() + delta.y)
    self.maparea:set_transform(transform)

    self.last_mouse_position = mouse_position
    self.moved = true
    return true
end

function ViewDrag.metatable:on_scroll_event(event)
end


function ViewDrag.new(maparea, x, y)
    local transform_xy = geo.vec2.new(
        maparea:get_transform():get_x(),
        maparea:get_transform():get_y())

    local drag = {}
    drag.maparea = maparea
    drag.moved = false
    drag.last_mouse_position = (
        maparea:screenspace_to_drawspace(geo.vec2.new(x, y))
        + transform_xy)
    setmetatable(drag, ViewDrag.metatable)
    return drag
end

return ViewDrag
