#version 330 core
// scene.frag - the Phong lighting model of the lecture "Shading" (the version
// with the halfway vector H, as in our Assignment 3), calculated for every
// pixel (Phong shading). Several point lights and two spot lights (sun/moon
// through the window and the desk lamp), emissive materials (screens, LEDs)
// and the RGB effects.

#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS 2

// Must match enum class Effect in src/scene/Material.h
const int FX_RGB    = 1;
const int FX_SCREEN = 2;
const int FX_UNLIT  = 5;
const int FX_KEY    = 6;

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;
in float vPressed;

out vec4 FragColor;

struct PointLight {
    vec3 position;
    vec3 color;       // colour * intensity
    float range;
};

struct SpotLight {
    vec3 position;
    vec3 direction;   // direction the light travels (normalized)
    vec3 color;       // colour * intensity
    float range;
    float cutoffCos;  // cos of the half-angle of the cone
    float exponent;
};

uniform PointLight uPoints[MAX_POINT_LIGHTS];
uniform int uPointCount;
uniform SpotLight uSpots[MAX_SPOT_LIGHTS];
uniform int uSpotCount;
uniform vec3 uAmbient;
uniform vec3 uViewPos;
uniform float uTime;

// Material
uniform vec3 uDiffuse;        // kd (also used as ka)
uniform float uSpecular;      // ks
uniform float uShininess;
uniform float uAlpha;
uniform bool uUseTexture;
uniform sampler2D uTexture;
uniform int uEffect;
uniform float uRgbPhase;
uniform float uScreenBrightness;

// RGB lighting (the same values for the keyboard, LED strips and PC fans)
uniform int   uRgbMode;       // 0 wave, 1 spectrum, 2 breathing, 3 static, 4 off
uniform float uRgbSpeed;
uniform vec3  uRgbStatic;
uniform float uRgbLevel;      // overall brightness, rises in "gaming mode"

// HSV -> RGB (lecture "Introduction", colour models). The hue circle is cut
// into 6 sectors of 60 degrees. The same function is in src/util/Rgb.h.
vec3 hsv2rgb(float h, float s, float v)
{
    float sector = fract(h) * 6.0;
    int i = int(sector);
    float f = sector - float(i);   // position inside the sector
    float p = v * (1.0 - s);
    float q = v * (1.0 - s * f);
    float t = v * (1.0 - s * (1.0 - f));
    if (i == 0) return vec3(v, t, p);
    if (i == 1) return vec3(q, v, p);
    if (i == 2) return vec3(p, v, t);
    if (i == 3) return vec3(p, q, v);
    if (i == 4) return vec3(t, p, v);
    return vec3(v, p, q);
}

// Colour of an LED. phase = position along the strip, so the colours move along it.
vec3 rgbEffect(float phase)
{
    float t = uTime * uRgbSpeed;
    if (uRgbMode == 0) return hsv2rgb(t * 0.12 + phase, 1.0, 1.0);   // rainbow wave
    if (uRgbMode == 1) return hsv2rgb(t * 0.08, 1.0, 1.0);           // whole spectrum
    if (uRgbMode == 2) {                                            // breathing
        float b = 0.12 + 0.88 * (0.5 + 0.5 * sin(t * 2.0));
        return hsv2rgb(t * 0.02, 1.0, b);
    }
    if (uRgbMode == 3) return uRgbStatic;                            // static colour
    return vec3(0.0);                                                // off
}

// Distance attenuation 1 / (a + b*d + c*d^2) from the lecture, with a = 1.
// b and c are chosen from the range of the light: at d = range the light is
// 1 / (1 + 1 + 10) = 1/12 of its full intensity.
float attenuation(float d, float range)
{
    float b = 1.0 / range;
    float c = 10.0 / (range * range);
    return 1.0 / (1.0 + b * d + c * d * d);
}

// Diffuse + specular light of one source:
// kd * I * max(N.L, 0) + ks * I * max(N.H, 0)^shininess, where H = normalize(L + V).
vec3 phong(vec3 N, vec3 V, vec3 L, vec3 light, vec3 kd)
{
    float NdotL = max(dot(N, L), 0.0);
    vec3 H = normalize(L + V);
    float spec = NdotL > 0.0 ? pow(max(dot(N, H), 0.0), uShininess) : 0.0;
    return light * (kd * NdotL + vec3(uSpecular * spec));
}

void main()
{
    vec3 kd = uDiffuse;
    float alpha = uAlpha;
    vec3 emissive = vec3(0.0);   // light the surface gives off itself (screens, LEDs)

    if (uEffect == FX_SCREEN) {
        emissive += texture(uTexture, vUV).rgb * uScreenBrightness;
        kd = vec3(0.01);
    } else if (uUseTexture) {
        vec4 t = texture(uTexture, vUV);
        kd *= t.rgb;
        alpha *= t.a;
    }

    if (uEffect == FX_RGB) {
        emissive += rgbEffect(vWorldPos.x * 0.6 + uRgbPhase) * uRgbLevel * 0.9;
    } else if (uEffect == FX_KEY) {
        // Keycap tops glow (legends lit from below), the sides get a little light.
        float glow = normalize(vNormal).y > 0.5 ? 0.5 : 0.1;
        emissive += rgbEffect(vUV.x) * uRgbLevel * (1.0 + vPressed * 1.5) * glow;
    } else if (uEffect == FX_UNLIT) {
        FragColor = vec4(min(kd + emissive, vec3(1.0)), alpha);   // bulbs: no lighting
        return;
    }

    vec3 N = normalize(vNormal);
    if (!gl_FrontFacing) N = -N;   // a surface seen from the back
    vec3 V = normalize(uViewPos - vWorldPos);

    vec3 color = uAmbient * kd;    // ambient part (ka = kd)

    for (int i = 0; i < uPointCount; ++i) {
        vec3 toLight = uPoints[i].position - vWorldPos;
        float d = length(toLight);
        vec3 light = uPoints[i].color * attenuation(d, uPoints[i].range);
        color += phong(N, V, toLight / d, light, kd);
    }

    for (int i = 0; i < uSpotCount; ++i) {
        vec3 toLight = uSpots[i].position - vWorldPos;
        float d = length(toLight);
        vec3 L = toLight / d;
        // Spot light: 0 outside the cone, (cos angle)^exponent inside it.
        float cosAngle = dot(-L, uSpots[i].direction);
        float spot = cosAngle > uSpots[i].cutoffCos ? pow(cosAngle, uSpots[i].exponent) : 0.0;
        vec3 light = uSpots[i].color * spot * attenuation(d, uSpots[i].range);
        color += phong(N, V, L, light, kd);
    }

    color += emissive;
    FragColor = vec4(min(color, vec3(1.0)), alpha);   // too bright -> 1 (lecture: MIN(I, 1))
}
