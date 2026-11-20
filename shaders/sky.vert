#version 330 core
// sky.vert - the big quad outside the window. Only passes the world position on;
// the fragment shader turns it into a view direction.

layout(location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vWorldPos;

void main()
{
    vec4 world = uModel * vec4(aPos, 1.0);
    vWorldPos = world.xyz;
    gl_Position = uProjection * uView * world;
}
