// map.frag - Map fragment shader.
// Copyright (C) 2022 Trevor Last
#version 430 core

in vec3 fNormal;
in vec2 fTexCoords;

out vec4 FragColor;

uniform sampler2D tex;
uniform vec3 modulate;


void main()
{
    float xLightFactor = fma(dot(fNormal, vec3(1.0, 0.0, 0.0)), 0.5, 0.5);
    float yLightFactor = fma(dot(fNormal, vec3(0.0, 1.0, 0.0)), 0.5, 0.5);
    float zLightFactor = fma(dot(fNormal, vec3(0.0, 0.0, 1.0)), 0.5, 0.5);

    float xColor = mix(0.0, 0.5, xLightFactor);
    float zColor = mix(0.25, 0.5, zLightFactor);
    float color = xColor + zColor;

    // FragColor = texture(tex, fTexCoords) * vec4(modulate, 1);
    FragColor = vec4(color * modulate, 1.0);
}
