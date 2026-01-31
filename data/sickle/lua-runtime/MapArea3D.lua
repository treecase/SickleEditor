-- degrees/second
local TURN_RATE = 120.0
-- degrees
local FOV_DELTA = 1.0


-- Keyboard key pressed.
function maparea3d.metatable:on_key_press_event(keyval)
    local state = self:get_state()
    local move = state:get_move_direction()
    local turn = state:get_turn_rates()

    -- Camera translation
    if keyval == LuaGDK.GDK_KEY_a or keyval == LuaGDK.GDK_KEY_A then
        move.x = 1
    elseif keyval == LuaGDK.GDK_KEY_d or keyval == LuaGDK.GDK_KEY_D then
        move.x = -1
    elseif keyval == LuaGDK.GDK_KEY_q or keyval == LuaGDK.GDK_KEY_Q then
        move.y = 1
    elseif keyval == LuaGDK.GDK_KEY_e or keyval == LuaGDK.GDK_KEY_E then
        move.y = -1
    elseif keyval == LuaGDK.GDK_KEY_w or keyval == LuaGDK.GDK_KEY_W then
        move.z = 1
    elseif keyval == LuaGDK.GDK_KEY_s or keyval == LuaGDK.GDK_KEY_S then
        move.z = -1

    -- Camera rotation
    elseif keyval == LuaGDK.GDK_KEY_Up then
        turn.y = -TURN_RATE
    elseif keyval == LuaGDK.GDK_KEY_Down then
        turn.y = TURN_RATE
    elseif keyval == LuaGDK.GDK_KEY_Left then
        turn.x = -TURN_RATE
    elseif keyval == LuaGDK.GDK_KEY_Right then
        turn.x = TURN_RATE

    -- Camera go fast
    elseif keyval == LuaGDK.GDK_KEY_Shift_L or keyval == LuaGDK.GDK_KEY_Shift_R then
        state:set_gofast(true)

    -- Multiselect
    elseif keyval == LuaGDK.GDK_KEY_Control_L or keyval == LuaGDK.GDK_KEY_Control_R then
        state:set_multiselect(true)

    elseif keyval == LuaGDK.GDK_KEY_z or keyval == LuaGDK.GDK_KEY_Z then
        self:set_wireframe(not self:get_wireframe())

    else
        return false
    end

    state:set_move_direction(move)
    state:set_turn_rates(turn)
    self:set_state(state)
    return true
end


-- Keyboard key released.
function maparea3d.metatable:on_key_release_event(keyval)
    local state = self:get_state()
    local move = state:get_move_direction()
    local turn = state:get_turn_rates()

    -- Camera translation
    if keyval == LuaGDK.GDK_KEY_a or keyval == LuaGDK.GDK_KEY_A then
        move.x = math.min(0, move.x)
    elseif keyval == LuaGDK.GDK_KEY_d or keyval == LuaGDK.GDK_KEY_D then
        move.x = math.max(0, move.x)
    elseif keyval == LuaGDK.GDK_KEY_q or keyval == LuaGDK.GDK_KEY_Q then
        move.y = math.min(0, move.y)
    elseif keyval == LuaGDK.GDK_KEY_e or keyval == LuaGDK.GDK_KEY_E then
        move.y = math.max(0, move.y)
    elseif keyval == LuaGDK.GDK_KEY_w or keyval == LuaGDK.GDK_KEY_W then
        move.z = math.min(0, move.z)
    elseif keyval == LuaGDK.GDK_KEY_s or keyval == LuaGDK.GDK_KEY_S then
        move.z = math.max(0, move.z)

    -- Camera rotation
    elseif keyval == LuaGDK.GDK_KEY_Up then
        turn.y = math.max(0, turn.y)
    elseif keyval == LuaGDK.GDK_KEY_Down then
        turn.y = math.min(0, turn.y)
    elseif keyval == LuaGDK.GDK_KEY_Left then
        turn.x = math.max(0, turn.x)
    elseif keyval == LuaGDK.GDK_KEY_Right then
        turn.x = math.min(0, turn.x)

    -- Camera go fast
    elseif keyval == LuaGDK.GDK_KEY_Shift_L or keyval == LuaGDK.GDK_KEY_Shift_R then
        state:set_gofast(false)

    -- Multiselect
    elseif keyval == LuaGDK.GDK_KEY_Control_L or keyval == LuaGDK.GDK_KEY_Control_R then
        state:set_multiselect(false)

    else
        return false
    end

    state:set_move_direction(move)
    state:set_turn_rates(turn)
    self:set_state(state)
    return true
end


-- Mouse button pressed.
function maparea3d.metatable:on_button_press_event(event)
    local state = self:get_state()
    -- Begin camera turn via middle mouse drag
    if event.button == 2 then
        state:set_pointer_prev(geo.vec2.new(event.x, event.y))
    else
        return false
    end
    self:set_state(state)
    return true
end


-- Mouse button released.
function maparea3d.metatable:on_button_release_event(event)
    -- Select/deselect a brush with left click.
    if event.button == 1 then
        local state = self:get_state()
        local editor = self:get_editor()

        if not state:get_multiselect() then
            editor:get_selection():clear()
        end

        local picked = self:pick_object(geo.vec2.new(event.x, event.y))
        if picked then
            if picked:is_selected() then
                editor:get_selection():remove(picked)
            elseif editor:matches_mode(picked) then
                editor:get_selection():add(picked)
            end
        end
    else
        return false
    end
    return true
end


-- Mouse moved over widget.
function maparea3d.metatable:on_motion_notify_event(event)
    local camera = self:get_camera()
    local state = self:get_state()
    if event.state & LuaGDK.GDK_BUTTON2_MASK ~= 0 then
        local delta = geo.vec2.new(event.x, event.y) - state:get_pointer_prev()
        camera:rotate(delta * self:get_mouse_sensitivity())
        state:set_pointer_prev(geo.vec2.new(event.x, event.y))
    else
        return false
    end
    self:set_camera(camera)
    self:set_state(state)
    return true
end


-- Mouse scrolled on widget.
function maparea3d.metatable:on_scroll_event(event)
    local camera = self:get_camera()
    if event.state & LuaGDK.GDK_MOD1_MASK ~= 0 then
        if event.direction == LuaGDK.GDK_SCROLL_DOWN then
            camera:set_fov(camera:get_fov() + FOV_DELTA)
        elseif event.direction == LuaGDK.GDK_SCROLL_UP then
            camera:set_fov(camera:get_fov() - FOV_DELTA)
        else
            return false
        end
    else
        return false
    end
    self:set_camera(camera)
    return true
end
