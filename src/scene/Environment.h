// Environment.h - time of day (sun/moon, sky colours) and the smooth fades of
// every light level, including the "gaming mode" transition.
#pragma once

#include "scene/Settings.h"

#include "core/Math.h"

struct Environment {
    // Derived from the time of day
    float day = 1.0f;                 // 0 = night, 1 = full day
    Vec3 sunDir{0.0f, 1.0f, 0.0f};   // towards the sun (moon at night)
    Vec3 windowLightColor{1.0f};
    float windowLightIntensity = 0.0f;
    Vec3 skyZenith{0.3f, 0.5f, 0.9f};
    Vec3 skyHorizon{0.7f, 0.8f, 0.95f};
    Vec3 sunDiscColor{1.0f};
    float stars = 0.0f;

    // Animated 0..1 levels (they ease towards their targets)
    float ceiling = 1.0f;
    float lamp = 1.0f;
    float blinds = 0.0f;              // 0 open, 1 fully closed
    float gaming = 0.0f;
    float rgb = 1.0f;
    float screen = 1.0f;

    // Instant change (used on startup).
    void snap(const SceneSettings& s);
    void update(float dt, SceneSettings& s);

private:
    void computeSky(float hours);
};
