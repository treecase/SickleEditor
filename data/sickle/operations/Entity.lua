add_operation(
    "Entity",
    "Create",
    "entity",
    {"classname"},
    function(editor, _, classname)
        pcall(editor.add_entity, editor, classname)
    end)