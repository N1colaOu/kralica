#version 330 core

layout(location = 0) in vec3  aPosition;
layout(location = 1) in vec3  aColor;
layout(location = 2) in float aSize;    // world-space radius

uniform mat4  uView;
uniform mat4  uProjection;
uniform float uPixelScale;
uniform float uPointScale;

out vec3 vColor;

void main() {
    vec4 viewPos = uView * vec4(aPosition, 1.0);
    gl_Position  = uProjection * viewPos;

    float dist = max(-viewPos.z, 0.001);
    float pixels = aSize * uPixelScale * uPointScale / dist;
    gl_PointSize = clamp(pixels, 1.0, 160.0);

    vColor = aColor;
}