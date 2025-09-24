#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec4 outColor;


float sdCircle(vec2 p, float r)
{
    return length(p) - r;
}

void main() {
    vec2 screenCoords = uv * 2 -1;
    float sdf=clamp(1-sdCircle(screenCoords, 0.0), 0, 1);
    vec3 color=vec3(0, 0.7, 0.7)*  sdf;
    outColor =vec4(uv.x,0,uv.y, 0);
}

