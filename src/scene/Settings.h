// Settings.h - everything the user can change with keys or the ImGui panel.
#pragma once

#include "core/Math.h"

enum class ScreenContent : int { Code = 0, Terminal, Docs, CameraFeed, Off, Count };
inline const char* kScreenContentNames[] = {"Code editor", "Terminal", "Docs (browser)", "Security camera", "Off"};

enum class Arrangement : int { Single = 0, Dual, Triple, Portrait, Count };
inline const char* kArrangementNames[] = {"Single", "Dual", "Triple", "Main + portrait"};

inline const char* kRgbModeNames[] = {"Rainbow wave", "Spectrum cycle", "Breathing", "Static colour", "Off"};
constexpr int kRgbModeCount = 5;

struct SceneSettings {
    // Time of day and lights
    float timeOfDay = 15.5f;     // hours, 0..24
    bool autoTime = false;
    float timeSpeed = 1.0f;      // in-game hours per real second
    bool ceilingLight = true;
    bool deskLamp = true;
    bool gamingMode = false;     // "lights out, RGB on" - the wow moment
    Vec3 lampColor{1.0f, 0.9f, 0.78f};
    float lampIntensity = 1.5f;

    // Desk lamp joints (degrees) - a 4-level hierarchy
    float lampYaw = 95.0f;       // base turns towards the keyboard
    float lampLowerArm = 35.0f;
    float lampUpperArm = 75.0f;
    float lampHead = -125.0f;

    // RGB
    int rgbMode = 0;
    float rgbSpeed = 1.0f;
    Vec3 rgbStatic{0.42f, 0.76f, 1.0f};
    float rgbBrightness = 1.0f;

    // Monitors
    Arrangement arrangement = Arrangement::Triple;
    ScreenContent content[3] = {ScreenContent::Code, ScreenContent::CameraFeed, ScreenContent::Terminal};
    float screenBrightness = 1.0f;

    // Items that can be shown / hidden
    bool showPlant = true;
    bool showMug = true;
    bool showPC = true;
    bool showChair = true;
    bool showLamp = true;
    bool showShelf = true;
    bool showPoster = true;
    bool showRug = true;
    bool showSecurityCam = true;

    // Colours
    Vec3 deskTint{1.0f};
    Vec3 chairColor{0.18f, 0.18f, 0.19f};
    Vec3 chairAccent{0.79f, 0.23f, 0.28f};
    Vec3 wallColor{0.9f, 0.9f, 0.92f};
    float chairYaw = -18.0f;
};
