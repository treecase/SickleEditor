
local moremath = require "moremath"
local EventListener = require "EventListener"


function round_to_grid(x)
    return moremath.round_to(x, gAppWin:get_grid_size())
end


EventListener.inherit(appwin.metatable)

function appwin.metatable:on_key_press_event(keyval)
    return false
end


function appwin.metatable:on_maptool_changed()
    self:doEvent("on_maptool_changed")
end


EventListener.construct(gAppWin)

gAppWin:add_maptool(
    "Select",
    {
        {"Delete", "Object.Delete"},
    },
    function(editor) return not editor:get_selection():is_empty() end
)
gAppWin:add_maptool(
    "Create Brush",
    {
        {"Create Brush", "Brush.CreateFromBox"},
    },
    function(editor) return true end
)
