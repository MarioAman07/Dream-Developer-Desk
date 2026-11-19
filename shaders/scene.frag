#version 330 core
// scene.frag - material colour (times the texture) with a simple fixed light,
// so that the shapes are visible. Screens give off their own light.

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;

out vec4 FragColor;

// Material
uniform vec3 uDiffuse;        // kd (also used as ka)
uniform float uAlpha;
uniform bool uUseTexture;
uniform sampler2D uTexture;

void main()
{
    vec3 kd = uDiffuse;
    float alpha = uAlpha;
    vec3 emissive = vec3(0.0);   // light the surface gives off itself (screens, LEDs)

    if (uUseTexture) {
        vec4 t = texture(uTexture, vUV);
        kd *= t.rgb;
        alpha *= t.a;
    }

    vec3 N = normalize(vNormal);
    if (!gl_FrontFacing) N = -N;   // a surface seen from the back

    // Simple fixed light from above, so that the shapes are visible.
    vec3 color = kd * (0.35 + 0.65 * max(dot(N, normalize(vec3(0.4, 1.0, 0.3))), 0.0));

    color += emissive;
    FragColor = vec4(min(color, vec3(1.0)), alpha);   // too bright -> 1 (lecture: MIN(I, 1))
}
