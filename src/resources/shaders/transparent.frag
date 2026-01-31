// Copyright (C) 2024 Trevor Last
#version 430 core

in vec2 fTexCoords;

out vec4 FragColor;

uniform sampler2D tex;
uniform vec3 modulate;


void main()
{
    vec4 color = texture(tex, fTexCoords) * vec4(modulate, 1);
    if (color.a <= 0.5)
        discard;
    FragColor = color;
}
