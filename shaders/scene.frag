#version 330 core
// scene.frag - every object in one colour for now.

out vec4 FragColor;

uniform vec3 uDiffuse;

void main()
{
    FragColor = vec4(uDiffuse, 1.0);
}
