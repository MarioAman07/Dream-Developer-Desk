// Material.h - surface parameters sent to scene.frag for every draw call.
#pragma once

#include "core/Math.h"

class Texture;

// Special shader paths. Must match the FX_* constants in shaders/scene.vert and scene.frag.
enum class Effect : int {
    None = 0,
};

struct Material {
    Vec3 albedo{0.8f};          // diffuse colour (kd); the ambient colour is the same
    float specular = 0.25f;     // ks: strength of the white highlight
    float shininess = 32.0f;    // Phong exponent (alpha in the lecture)
    float alpha = 1.0f;         // opacity; < 1 = drawn in the transparent pass

    const Texture* texture = nullptr;

    Effect effect = Effect::None;
};
