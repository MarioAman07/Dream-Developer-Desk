#version 330 core
// scene.vert - vertex shader for every object of the scene.

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

// Must match enum class Effect in src/scene/Material.h
const int FX_SWAY = 4;

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

void main()
{
    vec3 pos = aPos;

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
