#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 uv;

void main() {

    gl_Position = vec4(((inPosition / vec2(800,400))*2 -1), 0.0, 1.0);
    fragColor = inColor;
    uv=inUV;
}

