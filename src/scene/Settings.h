// Settings.h - parameters of the scene that can be changed while the program runs.
#pragma once

#include "core/Math.h"

enum class ScreenContent : int { Code = 0, Terminal, Docs, Off, Count };
inline const char* kScreenContentNames[] = {"Code editor", "Terminal", "Docs (browser)", "Off"};

enum class Arrangement : int { Single = 0, Dual, Triple, Portrait, Count };
inline const char* kArrangementNames[] = {"Single", "Dual", "Triple", "Main + portrait"};

struct SceneSettings {
    // Lights
    bool ceilingLight = true;
    bool deskLamp = true;
    Vec3 lampColor{1.0f, 0.9f, 0.78f};
    float lampIntensity = 1.5f;

    // Desk lamp joints (degrees) - a 4-level hierarchy
    float lampYaw = 95.0f;       // base turns towards the keyboard
    float lampLowerArm = 35.0f;
    float lampUpperArm = 75.0f;
    float lampHead = -125.0f;

    // Monitors
    Arrangement arrangement = Arrangement::Triple;
    ScreenContent content[3] = {ScreenContent::Code, ScreenContent::Docs, ScreenContent::Terminal};
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

    // Colours
    Vec3 deskTint{1.0f};
    Vec3 chairColor{0.18f, 0.18f, 0.19f};
    Vec3 chairAccent{0.79f, 0.23f, 0.28f};
    Vec3 wallColor{0.9f, 0.9f, 0.92f};
    float chairYaw = -18.0f;
};
