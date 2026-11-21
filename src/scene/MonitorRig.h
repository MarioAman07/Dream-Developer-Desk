// MonitorRig.h - up to three monitors on stands. Each monitor is a small
// hierarchy (stand -> pole -> hinge -> panel -> screen). Switching the
// arrangement animates every monitor to its new pose (slide, turn, rotate to
// portrait, shrink away when not used).
#pragma once

#include "scene/Settings.h"

#include "core/Math.h"

class Mesh;
class SceneNode;
class Texture;

struct MonitorPose {
    Vec2 xz{0.0f};    // position on the desk
    float yaw = 0.0f;      // degrees
    float roll = 0.0f;     // 0 landscape, 90 portrait
    float scale = 1.0f;    // 0 = put away
};

struct ScreenTextures {
    const Texture* code = nullptr;
    const Texture* terminal = nullptr;
    const Texture* docs = nullptr;
    unsigned int feed = 0;   // GL texture of the security camera framebuffer
};

class MonitorRig {
public:
    static constexpr int kCount = 3;

    void build(SceneNode* deskTop, const Mesh* cube, const Mesh* quad, const Mesh* cylinder, const Mesh* sphere);
    void snapTo(Arrangement a);   // no animation
    void update(float dt, const SceneSettings& s, const ScreenTextures& tex);

    bool isVisible(int i) const;
    bool showsFeed(int i, const SceneSettings& s) const;
    SceneNode* screen(int i) const { return m_monitors[i].screen; }

private:
    struct Monitor {
        SceneNode* root = nullptr;    // stand position + yaw
        SceneNode* panel = nullptr;   // roll (portrait)
        SceneNode* screen = nullptr;
        MonitorPose pose;
    };

    static MonitorPose targetPose(Arrangement a, int index);

    Monitor m_monitors[kCount];
};
