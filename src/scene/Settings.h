// Settings.h - parameters of the scene that can be changed while the program runs.
#pragma once

#include "core/Math.h"

struct SceneSettings {
    // Desk lamp joints (degrees) - a 4-level hierarchy
    float lampYaw = 95.0f;       // base turns towards the keyboard
    float lampLowerArm = 35.0f;
    float lampUpperArm = 75.0f;
    float lampHead = -125.0f;

    // Items that can be shown / hidden
    bool showMug = true;
    bool showLamp = true;
    bool showShelf = true;
    bool showPoster = true;
    bool showRug = true;

    // Colours
    Vec3 deskTint{1.0f};
    Vec3 wallColor{0.9f, 0.9f, 0.92f};
};
