// Material.h - surface parameters sent to scene.frag for every draw call.
#pragma once

#include "core/Math.h"

class Texture;

// Special shader paths. Must match the FX_* constants in shaders/scene.vert and scene.frag.
enum class Effect : int {
    None = 0,
    Screen = 2,      // emissive texture (monitor content)
    Sway = 4,        // vertex animation for plant leaves
    Unlit = 5,       // plain colour, no lighting (bulbs)
};

struct Material {
    Vec3 albedo{0.8f};          // diffuse colour (kd); the ambient colour is the same
    float specular = 0.25f;     // ks: strength of the white highlight
    float shininess = 32.0f;    // Phong exponent (alpha in the lecture)
    float alpha = 1.0f;         // opacity; < 1 = drawn in the transparent pass

    const Texture* texture = nullptr;
    Vec2 uvScrollSpeed{0.0f}; // UV units per second (scrolling code)
    bool rotateUV = false;         // portrait monitors

    Effect effect = Effect::None;
};
