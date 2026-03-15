// map.frag - Map fragment shader.
// Copyright (C) 2022 Trevor Last
#version 430 core

in vec3 fNormal;
in vec2 fTexCoords;

out vec4 FragColor;

uniform sampler2DArray tex;
uniform vec3 modulate;

readonly restrict layout(std430, binding=0) buffer TextureInfo {
    ivec2 texture_size[];
};

readonly restrict layout(std430, binding=1) buffer PrimitiveInfo {
    int texture_idx[];
};


void main()
{
    int TEXTURE_IDX = texture_idx[gl_PrimitiveID];

    float xLightFactor = fma(dot(fNormal, vec3(1.0, 0.0, 0.0)), 0.5, 0.5);
    float yLightFactor = fma(dot(fNormal, vec3(0.0, 1.0, 0.0)), 0.5, 0.5);
    float zLightFactor = fma(dot(fNormal, vec3(0.0, 0.0, 1.0)), 0.5, 0.5);

    float xLight = mix(0.0, 0.5, xLightFactor);
    float zLight = mix(0.25, 0.5, zLightFactor);
    float light = xLight + zLight;

    ivec3 texArrSize = textureSize(tex, 0);
    ivec2 texSize = texture_size[TEXTURE_IDX];

    vec3 texCoord = vec3(
        mod(fTexCoords.x, texSize.x) / texArrSize.x,
        mod(fTexCoords.y, texSize.y) / texArrSize.y,
        TEXTURE_IDX
    );

    FragColor = texture(tex, texCoord) * vec4(modulate, 1) * light;
}
