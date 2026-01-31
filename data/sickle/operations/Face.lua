add_operation(
    "Face", "SetTexture", "face", {"texture"},
    function(editor, faces, texture_id)
        for _,f in ipairs(faces) do
            f:set_texture(texture_id)
        end
    end)