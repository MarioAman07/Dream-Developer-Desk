// Environment.h - smooth fades of the light levels (lights switched on and off).
#pragma once

#include "scene/Settings.h"

#include "core/Math.h"

struct Environment {
    // Animated 0..1 levels (they ease towards their targets)
    float ceiling = 1.0f;
    float lamp = 1.0f;
    float rgb = 1.0f;
    float screen = 1.0f;

    // Instant change (used on startup).
    void snap(const SceneSettings& s);
    void update(float dt, SceneSettings& s);
};
