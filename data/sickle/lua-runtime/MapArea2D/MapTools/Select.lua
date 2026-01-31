-- Select
--
-- Select and transform brushes and entities.

local EventListener = require "EventListener"
local MoveSelected = require "MapArea2D/MapTools/Select/MoveSelected"
local ScaleDrag = require "MapArea2D/MapTools/Select/ScaleDrag"


local Select = {}
Select.metatable = {}
Select.metatable.__index = Select.metatable

EventListener.inherit(Select.metatable)


function Select.metatable:multiselect()
    return self.maparea.ctrl == true
end


function Select.metatable:on_removed()
    self.maparea:get_editor():get_selection():clear()
end


function Select.metatable:on_button_press_event(event)
    if self:doEvent("on_button_press_event", event) then return true end

    if event.button == 1 then
        local hovered = self.maparea:get_selection_box():check_point(
            self.maparea:screenspace_to_drawspace(
                geo.vec2.new(event.x, event.y)))

        if hovered == maparea2d.grabbablebox.CENTER then
            self:addListener(
                MoveSelected.new(self, self.maparea, event.x, event.y))
            return true

        elseif hovered == maparea2d.grabbablebox.NONE then
            ;

        else
            self:addListener(
                ScaleDrag.new(self, self.maparea, event.x, event.y, hovered))
            return true
        end
    end
end

function Select.metatable:on_button_release_event(event)
    if self:doEvent("on_button_release_event", event) then return true end

    if event.button == 1 then
        local editor = self.maparea:get_editor()

        if not self:multiselect() then
            editor:get_selection():clear()
        end

        local picked = self.maparea:pick_object(
            self.maparea:screenspace_to_drawspace(
                geo.vec2.new(event.x, event.y)))
        if picked then
            if picked:is_selected() then
                editor:get_selection():remove(picked)
            elseif editor:matches_mode(picked) then
                editor:get_selection():add(picked)
            end
        end
        return true
    end
end

function Select.metatable:on_key_press_event(event)
    if self:doEvent("on_key_press_event", event) then return true end
end

function Select.metatable:on_key_release_event(event)
    if self:doEvent("on_key_release_event", event) then return true end
end

function Select.metatable:on_motion_notify_event(event)
    if self:doEvent("on_motion_notify_event", event) then return true end
end

function Select.metatable:on_scroll_event(event)
    if self:doEvent("on_scroll_event", event) then return true end
end


function Select.new(maparea)
    local select = {}
    select.maparea = maparea
    setmetatable(select, Select.metatable)
    EventListener.construct(select)
    return select
end

return Select
