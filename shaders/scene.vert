#version 330 core
// scene.vert - vertex shader for every object of the scene.

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat3 uNormalMatrix;   // transpose(inverse(model)): correct normals under non-uniform scaling
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec2 uUVOffset;
uniform bool uRotateUV;       // portrait monitors: the content stays upright

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUV;

void main()
{
    vec3 pos = aPos;

    vec4 world = uModel * vec4(pos, 1.0);

    vWorldPos = world.xyz;
    vNormal = normalize(uNormalMatrix * aNormal);
    vec2 uv = uRotateUV ? vec2(1.0 - aUV.y, aUV.x) : aUV;
    vUV = uv + uUVOffset;
    gl_Position = uProjection * uView * world;
}
