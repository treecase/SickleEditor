-- CreateBrush.HandleDrag
--
-- BrushBox handle dragged.

local utils = require "MapArea2D/MapTools/CreateBrush/utils"


local HandleDrag = {}
HandleDrag.metatable = {}
HandleDrag.metatable.__index = HandleDrag.metatable


function HandleDrag.metatable:on_button_press_event(event)
    return true
end

function HandleDrag.metatable:on_button_release_event(event)
    self.parent:removeListener(self)
    return self.moved
end

function HandleDrag.metatable:on_key_press_event(event)
end

function HandleDrag.metatable:on_key_release_event(event)
end

function HandleDrag.metatable:on_motion_notify_event(event)
    local snapped = (self.maparea.alt ~= true)
    local mousepos = self.maparea:screenspace_to_drawspace(
        geo.vec2.new(event.x, event.y))
    if snapped then
        mousepos = geo.vec2.map(round_to_grid, mousepos)
    end

    if (    self.handle == maparea2d.grabbablebox.N
            or self.handle == maparea2d.grabbablebox.S) then
        mousepos.x = self.orig_cardinals.E.x
        self.anchor.x = self.orig_cardinals.W.x
    elseif (self.handle == maparea2d.grabbablebox.E
            or self.handle == maparea2d.grabbablebox.W) then
        mousepos.y = self.orig_cardinals.N.y
        self.anchor.y = self.orig_cardinals.S.y
    end

    local brushbox = self.maparea:get_editor():get_brushbox()
    brushbox:set_start(self.maparea:drawspace3_to_worldspace(self.anchor))
    brushbox:set_end(self.maparea:drawspace3_to_worldspace(
        geo.vec3.new(mousepos.x, mousepos.y, self.orig_cardinals.Z2)))
end


function HandleDrag.new(parent, maparea, x, y, handle)
    local northwest,southeast = utils.find_corners3(
        maparea:get_editor():get_brushbox(), maparea)
    local CARDINAL = {
        ["N"] = geo.vec2.new(0, northwest.y),
        ["E"] = geo.vec2.new(southeast.x, 0),
        ["S"] = geo.vec2.new(0, southeast.y),
        ["W"] = geo.vec2.new(northwest.x, 0),
        ["Z1"] = northwest.z,
        ["Z2"] = southeast.z,
    }
    local OPPOSITE_CORNER = {
        [maparea2d.grabbablebox.NE] = CARDINAL.S + CARDINAL.W,
        [maparea2d.grabbablebox.NW] = CARDINAL.S + CARDINAL.E,
        [maparea2d.grabbablebox.SE] = CARDINAL.N + CARDINAL.W,
        [maparea2d.grabbablebox.SW] = CARDINAL.N + CARDINAL.E,
        [maparea2d.grabbablebox.N] = CARDINAL.S,
        [maparea2d.grabbablebox.E] = CARDINAL.W,
        [maparea2d.grabbablebox.S] = CARDINAL.N,
        [maparea2d.grabbablebox.W] = CARDINAL.E,
    }
    local anchor = OPPOSITE_CORNER[handle]

    local drag = {}
    drag.parent = parent
    drag.maparea = maparea
    drag.moved = false
    drag.handle = handle
    drag.anchor = geo.vec3.new(anchor.x, anchor.y, CARDINAL.Z1)
    drag.orig_cardinals = CARDINAL
    setmetatable(drag, HandleDrag.metatable)
    return drag
end

return HandleDrag
