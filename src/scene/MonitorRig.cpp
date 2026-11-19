#include "scene/MonitorRig.h"

#include "scene/SceneNode.h"

#include <cmath>
#include <string>

namespace {
constexpr float kScreenW = 0.60f;     // 27" 16:9 panel
constexpr float kScreenH = 0.34f;
constexpr float kBezel = 0.012f;
constexpr float kHingeHeight = 0.37f;
} // namespace

MonitorPose MonitorRig::targetPose(Arrangement a, int i)
{
    // Positions are relative to the desk centre; the desk top's back edge is at z = -0.4.
    switch (a) {
    case Arrangement::Single: {
        const MonitorPose p[] = {{{0.0f, -0.20f}, 0, 0, 1.15f}, {{0.25f, -0.22f}, 0, 0, 0}, {{-0.25f, -0.22f}, 0, 0, 0}};
        return p[i];
    }
    case Arrangement::Dual: {
        const MonitorPose p[] = {{{-0.31f, -0.17f}, 10, 0, 1}, {{0.31f, -0.17f}, -10, 0, 1}, {{-0.31f, -0.22f}, 0, 0, 0}};
        return p[i];
    }
    case Arrangement::Triple: {
        const MonitorPose p[] = {{{0.0f, -0.20f}, 0, 0, 1}, {{0.60f, -0.07f}, -38, 0, 1}, {{-0.60f, -0.07f}, 38, 0, 1}};
        return p[i];
    }
    case Arrangement::Portrait:
    default: {
        const MonitorPose p[] = {{{-0.18f, -0.20f}, 6, 0, 1}, {{0.45f, -0.12f}, -22, 90, 1}, {{-0.60f, -0.10f}, 30, 0, 0}};
        return p[i];
    }
    }
}

void MonitorRig::build(SceneNode* deskTop, const Mesh* cube, const Mesh* quad, const Mesh* cylinder, const Mesh* sphere)
{
    Material metal;
    metal.albedo = Vec3(0.26f, 0.26f, 0.27f);
    metal.specular = 0.45f;
    metal.shininess = 96.0f;

    Material plastic;
    plastic.albedo = Vec3(0.22f, 0.22f, 0.23f);
    plastic.specular = 0.35f;
    plastic.shininess = 48.0f;

    Material led;
    led.albedo = Vec3(0.48f, 0.73f, 1.0f);

    for (int i = 0; i < kCount; ++i) {
        Monitor& m = m_monitors[i];
        std::string id = "monitor" + std::to_string(i);

        m.root = deskTop->addChild(id);
        m.root->addMesh(id + "_foot", cylinder, metal, {0.0f, 0.005f, 0.0f}, {0.22f, 0.01f, 0.16f});

        SceneNode* pole = m.root->addChild(id + "_pole");
        pole->position = Vec3(0.0f, 0.0f, -0.035f);
        pole->addMesh(id + "_pole_mesh", cube, metal, {0.0f, kHingeHeight * 0.5f, 0.0f}, {0.04f, kHingeHeight, 0.022f});

        SceneNode* hinge = pole->addChild(id + "_hinge");
        hinge->position = Vec3(0.0f, kHingeHeight, 0.03f);
        hinge->rotation = Vec3(-4.0f, 0.0f, 0.0f);   // slight backwards tilt

        m.panel = hinge->addChild(id + "_panel");
        m.panel->addMesh(id + "_housing", cube, plastic, {0.0f, 0.0f, 0.0f},
                         {kScreenW + 2 * kBezel, kScreenH + 2 * kBezel, 0.022f});
        m.panel->addMesh(id + "_back", cube, plastic, {0.0f, 0.0f, -0.02f}, {kScreenW * 0.55f, kScreenH * 0.6f, 0.03f});
        m.panel->addMesh(id + "_led", sphere, led, {kScreenW * 0.5f - 0.01f, -kScreenH * 0.5f - kBezel * 0.5f, 0.0115f},
                         Vec3(0.004f));

        Material screen;
        screen.effect = Effect::Screen;
        screen.specular = 0.03f;
        screen.shininess = 220.0f;
        m.screen = m.panel->addMesh(id + "_screen", quad, screen, {0.0f, 0.0f, 0.0118f}, {kScreenW, kScreenH, 1.0f});   // 0.8 mm in front of the housing
    }
    snapTo(Arrangement::Triple);
}

void MonitorRig::snapTo(Arrangement a)
{
    for (int i = 0; i < kCount; ++i) m_monitors[i].pose = targetPose(a, i);
    SceneSettings dummy;
    update(0.0f, dummy, ScreenTextures{});
}

bool MonitorRig::isVisible(int i) const
{
    return m_monitors[i].pose.scale > 0.02f;
}

void MonitorRig::update(float dt, const SceneSettings& s, const ScreenTextures& tex)
{
    for (int i = 0; i < kCount; ++i) {
        Monitor& m = m_monitors[i];
        MonitorPose goal = targetPose(s.arrangement, i);
        const float rate = 5.0f;
        m.pose.xz.x = ease(m.pose.xz.x, goal.xz.x, rate, dt);
        m.pose.xz.y = ease(m.pose.xz.y, goal.xz.y, rate, dt);
        m.pose.yaw = ease(m.pose.yaw, goal.yaw, rate, dt);
        m.pose.roll = ease(m.pose.roll, goal.roll, rate * 0.8f, dt);
        m.pose.scale = ease(m.pose.scale, goal.scale, rate, dt);

        m.root->position = Vec3(m.pose.xz.x, 0.0f, m.pose.xz.y);
        m.root->rotation = Vec3(0.0f, m.pose.yaw, 0.0f);
        m.root->scale = Vec3(m.pose.scale);
        m.root->visible = isVisible(i);
        m.panel->rotation = Vec3(0.0f, 0.0f, m.pose.roll);

        // Screen content
        Material& mat = m.screen->material;
        mat.rotateUV = m.pose.roll > 45.0f;
        mat.uvScrollSpeed = Vec2(0.0f);
        mat.texture = nullptr;
        mat.albedo = Vec3(0.12f);
        switch (s.content[i]) {
        case ScreenContent::Code:
            mat.effect = Effect::Screen;
            mat.texture = tex.code;
            mat.uvScrollSpeed = Vec2(0.0f, 0.025f);
            break;
        case ScreenContent::Terminal:
            mat.effect = Effect::Screen;
            mat.texture = tex.terminal;
            mat.uvScrollSpeed = Vec2(0.0f, 0.06f);
            break;
        case ScreenContent::Docs:
            mat.effect = Effect::Screen;
            mat.texture = tex.docs;
            break;
        default:
            mat.effect = Effect::None;   // switched off: dark glossy glass
            break;
        }
    }
}
