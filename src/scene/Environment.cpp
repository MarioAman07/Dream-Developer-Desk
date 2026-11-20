#include "scene/Environment.h"

namespace {
struct Targets {
    float ceiling, lamp, screen;
};

Targets targetsFor(const SceneSettings& s)
{
    return {
        s.ceilingLight ? 1.0f : 0.0f,
        s.deskLamp ? 1.0f : 0.0f,
        s.screenBrightness,
    };
}
} // namespace

void Environment::snap(const SceneSettings& s)
{
    Targets t = targetsFor(s);
    ceiling = t.ceiling;
    lamp = t.lamp;
    screen = t.screen;
}

void Environment::update(float dt, SceneSettings& s)
{
    // The light levels fade smoothly to their targets.
    Targets t = targetsFor(s);
    ceiling = ease(ceiling, t.ceiling, 4.0f, dt);
    lamp = ease(lamp, t.lamp, 6.0f, dt);
    screen = ease(screen, t.screen, 2.0f, dt);
}
