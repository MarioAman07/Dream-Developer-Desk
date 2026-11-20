#include "scene/Environment.h"

#include <algorithm>
#include <cmath>

namespace {
float smoothstep(float a, float b, float x)
{
    float t = std::clamp((x - a) / (b - a), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

struct Targets {
    float ceiling, lamp, blinds, gaming, rgb, screen;
};

Targets targetsFor(const SceneSettings& s)
{
    bool g = s.gamingMode;
    return {
        (s.ceilingLight && !g) ? 1.0f : 0.0f,
        (s.deskLamp && !g) ? 1.0f : 0.0f,
        g ? 1.0f : 0.0f,
        g ? 1.0f : 0.0f,
        (g ? 1.7f : 1.0f) * s.rgbBrightness,
        (g ? 1.25f : 1.0f) * s.screenBrightness,
    };
}
} // namespace

void Environment::computeSky(float hours)
{
    // 06:00 sunrise, 12:00 highest, 18:00 sunset.
    float a = (hours - 6.0f) / 12.0f * PI;
    float height = std::sin(a);                       // -1..1
    float elev = radians(58.0f) * height;
    float across = std::cos(a);                       // sun moves along the window

    Vec3 sun = normalize(Vec3(-std::cos(elev) * 0.9f, std::sin(elev), 0.55f * across));
    Vec3 moon = normalize(Vec3(-0.75f, 0.55f, -0.25f));

    day = smoothstep(-0.10f, 0.22f, height);
    float sunset = 1.0f - smoothstep(0.0f, 0.45f, std::abs(height));   // near the horizon
    stars = 1.0f - smoothstep(-0.18f, 0.04f, height);

    Vec3 sunColor = mix(Vec3(1.0f, 0.81f, 0.61f), Vec3(1.0f, 0.98f, 0.94f), smoothstep(0.05f, 0.5f, height));
    Vec3 moonColor(0.7f, 0.77f, 0.98f);

    sunDir = normalize(mix(moon, sun, day));
    windowLightColor = mix(moonColor, sunColor, day);
    windowLightIntensity = mix(0.5f, 2.2f, day);
    sunDiscColor = mix(Vec3(0.75f, 0.8f, 0.9f), sunColor, day);

    Vec3 dayZenith(0.6f, 0.78f, 0.89f), dayHorizon(0.85f, 0.9f, 0.93f);
    Vec3 duskZenith(0.58f, 0.55f, 0.75f), duskHorizon(0.9f, 0.73f, 0.6f);
    Vec3 nightZenith(0.045f, 0.07f, 0.15f), nightHorizon(0.17f, 0.21f, 0.36f);
    Vec3 zen = mix(nightZenith, dayZenith, day);
    Vec3 hor = mix(nightHorizon, dayHorizon, day);
    float dusk = sunset * smoothstep(-0.25f, 0.0f, height);
    skyZenith = mix(zen, duskZenith, dusk * 0.7f);
    skyHorizon = mix(hor, duskHorizon, dusk);
}

void Environment::snap(const SceneSettings& s)
{
    computeSky(s.timeOfDay);
    Targets t = targetsFor(s);
    ceiling = t.ceiling;
    lamp = t.lamp;
    blinds = t.blinds;
    gaming = t.gaming;
    rgb = t.rgb;
    screen = t.screen;
}

void Environment::update(float dt, SceneSettings& s)
{
    if (s.autoTime) {
        s.timeOfDay += s.timeSpeed * dt;
        if (s.timeOfDay >= 24.0f) s.timeOfDay -= 24.0f;
    }
    computeSky(s.timeOfDay);

    // Gaming mode sequence: blinds roll down, room lights fade out, RGB rises.
    Targets t = targetsFor(s);
    blinds = ease(blinds, t.blinds, 1.6f, dt);
    ceiling = ease(ceiling, t.ceiling, s.gamingMode ? 1.8f : 4.0f, dt);
    lamp = ease(lamp, t.lamp, s.gamingMode ? 2.2f : 6.0f, dt);
    gaming = ease(gaming, t.gaming, 1.5f, dt);
    rgb = ease(rgb, t.rgb, 1.2f, dt);
    screen = ease(screen, t.screen, 2.0f, dt);
}
