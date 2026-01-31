add_operation(
    "Object", "Delete",
    "object",
    {},
    function(editor, objects)
        for _,obj in ipairs(objects) do
            -- remove_object can throw if obj is non-deletable (eg. Faces).
            pcall(editor.remove_object, editor, obj)
        end
    end
)