-- ScaleDrag
--
-- Scale the selected brushes.
-- Hold Ctrl to scale from the center of the brush.

local ScaleDrag = {}
ScaleDrag.metatable = {}
ScaleDrag.metatable.__index = ScaleDrag.metatable


-- Get the centerpoint of a set of brushes.
--
-- find_center(brushes: iterable) -> geo.vec3
--
-- brushes: iterable for a collection of brushes
-- Returns the centerpoint of the brushes in worldspace
local function find_center(brushes)
    local center = geo.vec3.new()
    local count = 0.0
    for brush in brushes do
        for i,vertex in ipairs(brush:get_vertices()) do
            center = center + vertex
            count = count + 1
        end
    end
    return center / count
end


-- Find minimum and maximum coordinates for a set of brushes.
--
-- find_bounds2D(brushes: iterable) -> geo.vec2, geo.vec2
--
-- maparea: Maparea who's drawspace we're working in
-- brushes: iterable for a collection of brushes
-- Returns min, max coordinates in drawspace
local function find_bounds2D(maparea, brushes)
    local min = {x=nil, y=nil}
    local max = {x=nil, y=nil}
    for brush in brushes do
        for _,vws in ipairs(brush:get_vertices()) do
            local v = maparea:worldspace_to_drawspace(vws)
            if min.x == nil or v.x < min.x then min.x = v.x end
            if max.x == nil or v.x > max.x then max.x = v.x end
            if min.y == nil or v.y < min.y then min.y = v.y end
            if max.y == nil or v.y > max.y then max.y = v.y end
        end
    end
    return geo.vec2.new(min), geo.vec2.new(max)
end


-- find_opposite_corner(
--   drag: ScaleDrag,
--   corner: GrabbableBox.Area,
--   brushes: iterable) -> geo.vec2
--
-- maparea: MapArea2D whose drawspace we're working in
-- corner: Box area corner to find the opposite for
-- brushes: iterable for a collection of brushes
-- Returns position of corner opposite to CORNER in drawspace
local function find_opposite_corner(maparea, corner, brushes)
    local min, max = find_bounds2D(maparea, brushes)

    local DIRECTION_COORD = {
        ["N"] = min.y,
        ["E"] = max.x,
        ["S"] = max.y,
        ["W"] = min.x,
        ["x"] = min.x + (max.x - min.x) * 0.5,
        ["y"] = min.y + (max.y - min.y) * 0.5
    }
    local OPPOSITE_HANDLE = {
        [maparea2d.grabbablebox.NE] = maparea2d.grabbablebox.SW,
        [maparea2d.grabbablebox.NW] = maparea2d.grabbablebox.SE,
        [maparea2d.grabbablebox.SE] = maparea2d.grabbablebox.NW,
        [maparea2d.grabbablebox.SW] = maparea2d.grabbablebox.NE,
        [maparea2d.grabbablebox.N ] = maparea2d.grabbablebox.S,
        [maparea2d.grabbablebox.S ] = maparea2d.grabbablebox.N,
        [maparea2d.grabbablebox.E ] = maparea2d.grabbablebox.W,
        [maparea2d.grabbablebox.W ] = maparea2d.grabbablebox.E
    }
    -- Indexes into DIRECTION_COORD
    local DIRECTION_LABEL = {
        [maparea2d.grabbablebox.NE] = {"N", "E"},
        [maparea2d.grabbablebox.NW] = {"N", "W"},
        [maparea2d.grabbablebox.SE] = {"S", "E"},
        [maparea2d.grabbablebox.SW] = {"S", "W"},
        [maparea2d.grabbablebox.N ] = {"N", "x"},
        [maparea2d.grabbablebox.S ] = {"S", "x"},
        [maparea2d.grabbablebox.E ] = {"y", "E"},
        [maparea2d.grabbablebox.W ] = {"y", "W"}
    }

    local opposite_handle = OPPOSITE_HANDLE[corner]
    local opposite_label = DIRECTION_LABEL[opposite_handle]
    local opposite_corner = geo.vec2.new(
        DIRECTION_COORD[opposite_label[2]],
        DIRECTION_COORD[opposite_label[1]])

    return opposite_corner
end


-- Divide two vectors, skipping if the denominator element is 0.
--
-- safe_vector_divide2(numerator: geo.vec2, denominator: geo.vec2) -> geo.vec2
--
-- numerator: Numerator of the division
-- denominator: Denominator of the division
-- Return numerator / denominator, with any zeroes in denominator replaced with
-- ones.
local function safe_vector_divide2(numerator, denominator)
    local function make_safe(x)
        if x == 0.0 then
            return 1.0
        else
            return x
        end
    end
    return numerator / geo.vec2.map(make_safe, denominator)
end


-- Divide two vectors, skipping if the denominator element is 0.
--
-- safe_vector_divide3(numerator: geo.vec3, denominator: geo.vec3) -> geo.vec3
--
-- numerator: Numerator of the division
-- denominator: Denominator of the division
-- Return numerator / denominator, with any zeroes in denominator replaced with
-- ones.
local function safe_vector_divide3(numerator, denominator)
    local function make_safe(x)
        if x == 0.0 then
            return 1.0
        else
            return x
        end
    end
    return numerator / geo.vec3.map(make_safe, denominator)
end


-- Convert a 2D scaling vector from drawspace to a 3D one in worldspace.
--
-- scale2D_to_scale3D(scale: geo.vec2, maparea: MapArea) -> geo.vec3
--
-- scale: 2D scaling vector in drawspace
-- maparea: The maparea who's drawspace we're in
-- Return a 3D scaling vector in worldspace.
local function scale2D_to_scale3D(scale, maparea)
    local scale3D = maparea:drawspace_to_worldspace(scale)
    local axis = geo.vec2.new({x=1.0, y=1.0})
    local axis3 = maparea:drawspace_to_worldspace(axis)
    for _,d in ipairs({"x", "y", "z"}) do
        if scale3D[d] == 0 then
            if axis3[d] == 0 then
                scale3D[d] = 1
            else
                scale3D[d] = axis3[d]
            end
        end
    end
    return scale3D
end



function ScaleDrag.metatable:snapped()
    return self.maparea.alt == false
end

function ScaleDrag.metatable:centered()
    -- return self.maparea.ctrl == true
    return false
end


function ScaleDrag.metatable:on_button_press_event(event)
    return true
end

function ScaleDrag.metatable:on_button_release_event(event)
    self.parent:removeListener(self)
    return self.moved
end

function ScaleDrag.metatable:on_key_press_event(event)
end

function ScaleDrag.metatable:on_key_release_event(event)
end

function ScaleDrag.metatable:on_motion_notify_event(event)
    -- Scaling direction factors. Diagonal directions scale in both x and y,
    -- horizontal only in x, and vertical only in y.
    local SCALE_FACTORS = {
        [maparea2d.grabbablebox.NE] = {1, 1},
        [maparea2d.grabbablebox.NW] = {1, 1},
        [maparea2d.grabbablebox.SE] = {1, 1},
        [maparea2d.grabbablebox.SW] = {1, 1},
        [maparea2d.grabbablebox.N ] = {0, 1},
        [maparea2d.grabbablebox.E ] = {1, 0},
        [maparea2d.grabbablebox.S ] = {0, 1},
        [maparea2d.grabbablebox.W ] = {1, 0}
    }

    -- Get the scaling factor and convert it to worldspace?
    local scale_factors = geo.vec2.new(
        SCALE_FACTORS[self.handle][1],
        SCALE_FACTORS[self.handle][2])

    -- Get mouse position in drawspace.
    local mouse_pos = self.maparea:screenspace_to_drawspace(
        geo.vec2.new(event.x, event.y))

    -- Snap mouse position to grid if necessary.
    if self:snapped() then
        mouse_pos = geo.vec2.map(round_to_grid, mouse_pos)
    end


    local scale_info = self.corner
    if self:centered() then
        scale_info = self.center
    end

    -- Calculate distance between mouse and scaling origin.
    local distance = geo.vec2.map(math.abs, mouse_pos - scale_info.origin)

    -- Calculate scaling factor by dividing current distance by starting
    -- distance.
    local scale = safe_vector_divide2(distance, scale_info.base_distance)

    -- Lock scaling to appropriate axes.
    scale = scale * scale_factors

    -- Convert scaling factor to a worldspace vector.
    local scale3D = scale2D_to_scale3D(scale, self.maparea)
    -- Convert previous scaling factor to a worldspace vector.
    local prev_scale3D = scale2D_to_scale3D(self.prev_scale, self.maparea)
    -- Convert scale_origin to worldspace.
    local scale_origin3D = self.maparea:drawspace_to_worldspace(
        scale_info.origin)

    -- Generate the scaling matrix.
    local scale_mat = geo.matrix.scale(geo.matrix.new(), scale3D)

    -- Generate matrix to undo the previously applied scaling.
    local inverse_prev_scale_mat = geo.matrix.scale(
        geo.matrix.new(),
        safe_vector_divide3(1.0, prev_scale3D))

    -- Apply the transformations.
    for brush in self.maparea:get_editor():get_selection():iterate() do
        brush:transform(
            geo.matrix.translate(geo.matrix.new(), scale_origin3D)
            * scale_mat
            * inverse_prev_scale_mat
            * geo.matrix.translate(geo.matrix.new(), -scale_origin3D))
    end

    -- Update state.
    self.prev_scale = scale
    self.moved = true
    return true
end

function ScaleDrag.metatable:on_scroll_event(event)
end


function ScaleDrag.new(parent, maparea, x, y, handle)
    local scale_drag = {}
    scale_drag.parent = parent
    scale_drag.maparea = maparea
    scale_drag.handle = handle
    scale_drag.moved = false

    setmetatable(scale_drag, ScaleDrag.metatable)

    local click_pos = maparea:screenspace_to_drawspace(geo.vec2.new(x, y))
    if scale_drag:snapped() then
        click_pos = geo.vec2.map(round_to_grid, click_pos)
    end

    local center = maparea:worldspace_to_drawspace(
        find_center(maparea:get_editor():get_selection():iterate()))
    scale_drag.center = {
        origin = center,
        base_distance = geo.vec2.map(math.abs, click_pos - center)
    }

    local corner = find_opposite_corner(
        maparea,
        handle,
        maparea:get_editor():get_selection():iterate())
    scale_drag.corner = {
        origin = corner,
        base_distance = geo.vec2.map(math.abs, click_pos - corner)
    }
    scale_drag.prev_scale = geo.vec2.new(1, 1)

    return scale_drag
end

return ScaleDrag
