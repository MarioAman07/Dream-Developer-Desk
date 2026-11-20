#version 330 core
// scene.vert - vertex shader for every object of the scene.

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

// Must match enum class Effect in src/scene/Material.h
const int FX_SWAY = 4;
const int FX_KEY  = 6;

uniform mat4 uModel;
uniform mat3 uNormalMatrix;   // transpose(inverse(model)): correct normals under non-uniform scaling
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uTime;
uniform int  uEffect;
uniform vec2 uUVOffset;
uniform bool uRotateUV;       // portrait monitors: the content stays upright

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUV;
out float vPressed;           // keycaps: 1 while the key is "pressed", else 0

// Pseudo-random value in [0, 1), the same formula as lattice() in our Assignment 4.
float random(vec2 p)
{
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    vec3 pos = aPos;

    // Keycaps: aUV.x = phase of the RGB wave, aUV.y = number of the key.
    // "Ghost typing": 9 times per second every key gets a new random number and
    // the keys with a number above 0.965 are pressed down by 3.5 mm.
    vPressed = 0.0;
    if (uEffect == FX_KEY) {
        float slot = mod(floor(uTime * 9.0), 1000.0);
        vPressed = step(0.965, random(vec2(aUV.y * 1.7, slot)));
        pos.y -= vPressed * 0.0035;
    }

    vec4 world = uModel * vec4(pos, 1.0);

    // Plant leaves move like a wave (lecture "Procedural Methods": wave motion).
    // The tip of a leaf (top of the mesh) moves, the base stays in place.
    if (uEffect == FX_SWAY) {
        float h = clamp(aPos.y + 0.5, 0.0, 1.0);
        float phase = world.x * 4.0 + world.z * 3.0;
        world.x += sin(uTime * 1.6 + phase) * 0.012 * h;
        world.z += cos(uTime * 1.1 + phase) * 0.010 * h;
    }

    vWorldPos = world.xyz;
    vNormal = normalize(uNormalMatrix * aNormal);
    vec2 uv = uRotateUV ? vec2(1.0 - aUV.y, aUV.x) : aUV;
    vUV = uv + uUVOffset;
    gl_Position = uProjection * uView * world;
}
