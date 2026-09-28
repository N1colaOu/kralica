#version 330 core

in vec3 vColor;
uniform float uGlow;
out vec4 FragColor;

void main() {
    vec2  d  = gl_PointCoord - vec2(0.5);
    float r2 = dot(d, d);
    if (r2 > 0.25) discard;

    float falloff   = smoothstep(0.25, 0.0, r2);
    float intensity = (falloff * falloff + 0.25 * falloff) * uGlow;

    FragColor = vec4(vColor * intensity, 1.0);
}