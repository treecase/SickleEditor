add_operation(
    "Brush", "Transform", "brush", {"mat4"},
    function(editor, brushes, transformation)
        for _,b in ipairs(brushes) do
            b:transform(transformation)
        end
    end)

add_operation(
    "Brush", "Translate", "brush", {"vec3"},
    function(editor, brushes, delta)
        for _,b in ipairs(brushes) do
            b:translate(delta)
        end
    end)

add_operation(
    "Brush", "Rotate", "brush", {"f", "vec3"},
    function(editor, brushes, angle, axis)
        for _,b in ipairs(brushes) do
            local verts = b:get_vertices()
            local center = geo.vec3.new()
            local i = 0.0
            for _,vertex in ipairs(verts) do
                center = center + vertex
                i = i + 1.0
            end
            center = center / i

            b:translate(-center)
            b:rotate(angle, axis)
            b:translate(center)
        end
    end)

add_operation(
    "Brush", "Scale", "brush", {"vec3"},
    function(editor, brushes, scale)
        for _,b in ipairs(brushes) do
            b:scale(scale)
        end
    end,
    {geo.vec3.new(1, 1, 1)})

add_operation(
    "Brush",
    "CreateFromBox",
    "brush",
    {},
    function(editor, _)
        local bb = editor:get_brushbox()
        local a = bb:get_start()
        local b = bb:get_end()
        bb:set_start(geo.vec3.new())
        bb:set_end(geo.vec3.new())
        editor:add_brush(
            {a.x, a.y, a.z},
            {a.x, a.y, b.z},
            {a.x, b.y, a.z},
            {a.x, b.y, b.z},
            {b.x, a.y, a.z},
            {b.x, a.y, b.z},
            {b.x, b.y, a.z},
            {b.x, b.y, b.z}
        )
    end)