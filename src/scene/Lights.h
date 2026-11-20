// Lights.h - light data collected by the scene each frame and uploaded by the renderer.
#pragma once

#include "core/Math.h"

#include <vector>

constexpr int kMaxPointLights = 8;   // must match MAX_POINT_LIGHTS in scene.frag
constexpr int kMaxSpotLights = 2;    // must match MAX_SPOT_LIGHTS in scene.frag

struct PointLight {
    Vec3 position;
    Vec3 color{1.0f};
    float intensity = 1.0f;
    float range = 5.0f;        // the light is almost 0 at this distance (see attenuation in scene.frag)
    bool deskShadow = false;   // the desk top blocks this light (only big room lights;
                               // spot lights always use the desk shadow)
};

// Spot light as in the lecture "Shading": light only inside a cone around
// `direction`, brighter towards the centre: (cos angle)^exponent.
struct SpotLight {
    Vec3 position;
    Vec3 direction{0.0f, -1.0f, 0.0f};
    Vec3 color{1.0f};
    float intensity = 1.0f;
    float range = 8.0f;
    float cutoffDeg = 35.0f;   // half-angle of the cone
    float exponent = 4.0f;     // how fast the light falls off towards the edge
};

struct LightSet {
    Vec3 ambient{0.1f};
    // Desk top as the shadow caster: rectangle min/max x and z, y = height of the
    // plane in the middle of the board (see deskShadow in scene.frag).
    Vec3 deskMin;
    Vec3 deskMax;
    std::vector<PointLight> points;
    std::vector<SpotLight> spots;
};
