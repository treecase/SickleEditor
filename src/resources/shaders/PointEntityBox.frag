// Copyright (C) 2024 Trevor Last
#version 430 core

out vec4 FragColor;

uniform vec3 color;
uniform vec3 modulate;


void main()
{
    FragColor = vec4(color * modulate, 1);
}
