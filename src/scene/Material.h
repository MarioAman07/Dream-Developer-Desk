// Material.h - surface parameters sent to scene.frag for every draw call.
#pragma once

#include "core/Math.h"

class Texture;

// Special shader paths. Must match the FX_* constants in shaders/scene.vert and scene.frag.
enum class Effect : int {
    None = 0,
    RgbStrip = 1,    // emissive colour cycles along world X (LED strip, fan rings)
    Screen = 2,      // emissive texture (monitor content)
    CameraFeed = 3,  // render-to-texture feed with a CCTV look
    Sway = 4,        // vertex animation for plant leaves
    Unlit = 5,       // plain colour, no lighting (bulbs)
    KeyCap = 6,      // keyboard keycaps (RGB per key + "ghost typing" in the shaders)
};

struct Material {
    Vec3 albedo{0.8f};          // diffuse colour (kd); the ambient colour is the same
    float specular = 0.25f;     // ks: strength of the white highlight
    float shininess = 32.0f;    // Phong exponent (alpha in the lecture)
    float alpha = 1.0f;         // opacity; < 1 = drawn in the transparent pass

    const Texture* texture = nullptr;
    unsigned int rawTexture = 0;   // GL texture id (used for the camera feed)
    Vec2 uvScrollSpeed{0.0f}; // UV units per second (scrolling code)
    bool rotateUV = false;         // portrait monitors

    Effect effect = Effect::None;
    float rgbPhase = 0.0f;         // offset for RGB effects so objects differ
};
