// Copyright (C) 2024 Trevor Last
#version 430 core

layout(location=0) in vec2 vPos;
layout(location=1) in vec2 vTexCoords;

out vec2 fTexCoords;

uniform vec2 scale;
uniform vec3 position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    vec3 right = scale.x * normalize(vec3(view[0][0], view[1][0], view[2][0]));
    vec3 up = scale.y * normalize(vec3(view[0][1], view[1][1], view[2][1]));

    vec3 pos = (model * vec4(position, 1)).xyz + right * vPos.x + up * vPos.y;

    gl_Position = projection * view * vec4(pos, 1.0);
    fTexCoords = vTexCoords;
}
