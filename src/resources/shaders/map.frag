// map.frag - Map fragment shader.
// Copyright (C) 2022 Trevor Last
#version 430 core

in vec2 fTexCoords;

out vec4 FragColor;

uniform sampler2D tex;
uniform vec3 modulate;


void main()
{
    FragColor = texture(tex, fTexCoords) * vec4(modulate, 1);
}
