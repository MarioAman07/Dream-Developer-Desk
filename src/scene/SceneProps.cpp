// SceneProps.cpp - office chair, floor plant, gaming PC with spinning RGB fans
// and the security camera whose view is shown on a monitor.
#include "scene/DeskScene.h"

#include "gfx/Mesh.h"

#include <cmath>
#include <string>

namespace {
Material makeMat(const Vec3& albedo, float spec = 0.2f, float shin = 24.0f)
{
    Material m;
    m.albedo = albedo;
    m.specular = spec;
    m.shininess = shin;
    return m;
}
} // namespace

void DeskScene::buildChair()
{
    // Hierarchy: chair root -> 5 legs (+ casters) / gas lift -> swivel (seat) -> backrest, armrests
    m_chairRoot = m_root->addChild("chair");
    m_chairRoot->position = Vec3(0.12f, 0.0f, -0.95f);

    Material base = makeMat({0.28f, 0.28f, 0.29f}, 0.7f, 96.0f);
    for (int i = 0; i < 5; ++i) {
        SceneNode* leg = m_chairRoot->addChild("chair_leg" + std::to_string(i));
        leg->rotation.y = i * 72.0f + 18.0f;
        leg->addMesh("chair_leg_mesh", m_cube, base, {0, 0.075f, 0.15f}, {0.04f, 0.03f, 0.3f});
        leg->addMesh("chair_caster", m_sphere, base, {0, 0.025f, 0.29f}, Vec3(0.05f));
    }
    m_chairRoot->addMesh("chair_hub", m_cylinder, base, {0, 0.08f, 0}, {0.09f, 0.05f, 0.09f});
    m_chairRoot->addMesh("chair_lift", m_cylinder, makeMat({0.79f, 0.79f, 0.8f}, 0.9f, 128.0f), {0, 0.25f, 0}, {0.045f, 0.32f, 0.045f});

    m_chairSwivel = m_chairRoot->addChild("chair_swivel");
    m_chairSwivel->position = Vec3(0.0f, 0.42f, 0.0f);

    Material fabric = makeMat(settings.chairColor, 0.15f, 12.0f);
    Material accent = makeMat(settings.chairAccent, 0.3f, 24.0f);
    m_chairSwivel->addMesh("chair_plate", m_cube, base, {0, 0.01f, 0}, {0.22f, 0.02f, 0.22f});
    m_chairColored.push_back(m_chairSwivel->addMesh("chair_seat", m_cube, fabric, {0, 0.06f, 0}, {0.5f, 0.08f, 0.48f}));
    m_chairAccent.push_back(m_chairSwivel->addMesh("chair_seat_trim", m_cube, accent, {0, 0.06f, -0.241f}, {0.5f, 0.06f, 0.004f}));

    // Backrest leans back a little (rotation around X at its lower edge = the pivot).
    SceneNode* back = m_chairSwivel->addChild("chair_backrest");
    back->position = Vec3(0.0f, 0.1f, 0.22f);
    back->rotation.x = 10.0f;
    m_chairColored.push_back(back->addMesh("chair_back", m_cube, fabric, {0, 0.34f, 0}, {0.48f, 0.6f, 0.07f}));
    m_chairColored.push_back(back->addMesh("chair_head", m_cube, fabric, {0, 0.73f, 0}, {0.3f, 0.14f, 0.07f}));
    for (float x : {-0.12f, 0.12f})
        m_chairAccent.push_back(back->addMesh("chair_stripe", m_cube, accent, {x, 0.34f, -0.036f}, {0.035f, 0.56f, 0.004f}));

    for (float side : {-1.0f, 1.0f}) {
        SceneNode* arm = m_chairSwivel->addChild("chair_arm");
        arm->position = Vec3(side * 0.27f, 0.08f, 0.04f);
        arm->addMesh("chair_arm_post", m_cube, base, {0, 0.1f, 0}, {0.035f, 0.2f, 0.035f});
        m_chairColored.push_back(arm->addMesh("chair_arm_pad", m_cube, fabric, {0, 0.21f, -0.02f}, {0.07f, 0.03f, 0.26f}));
    }
}

void DeskScene::buildPlant()
{
    // Pot -> stems -> leaves; leaves sway in the vertex shader (Effect::Sway).
    m_plant = m_root->addChild("plant");
    m_plant->position = Vec3(-2.45f, 0.0f, -2.05f);

    m_plant->addMesh("plant_pot", m_cylinder, makeMat({0.8f, 0.6f, 0.5f}, 0.15f, 16.0f), {0, 0.19f, 0}, {0.36f, 0.38f, 0.36f});
    m_plant->addMesh("plant_pot_rim", m_cylinder, makeMat({0.76f, 0.57f, 0.47f}, 0.15f, 16.0f), {0, 0.38f, 0}, {0.39f, 0.04f, 0.39f});
    // The soil is 2 mm higher than the top of the rim: two surfaces at the same depth
    // would fight in the depth buffer (z-fighting = flickering stripes).
    m_plant->addMesh("plant_soil", m_cylinder, makeMat({0.38f, 0.32f, 0.26f}, 0.02f, 4.0f), {0, 0.392f, 0}, {0.33f, 0.02f, 0.33f});

    const Vec3 greens[] = {{0.35f, 0.62f, 0.38f}, {0.41f, 0.67f, 0.42f}, {0.32f, 0.58f, 0.35f}};
    Material stemMat = makeMat({0.42f, 0.62f, 0.38f}, 0.1f, 8.0f);
    const int leaves = 13;
    for (int i = 0; i < leaves; ++i) {
        float t = static_cast<float>(i) / leaves;
        SceneNode* stem = m_plant->addChild("plant_stem" + std::to_string(i));
        stem->position = Vec3(0.0f, 0.39f, 0.0f);
        stem->rotation = Vec3(18.0f + 32.0f * std::fmod(t * 3.7f, 1.0f), i * 137.5f, 0.0f);   // golden angle spiral
        float len = 0.35f + 0.45f * std::fmod(t * 5.3f, 1.0f);
        stem->addMesh("plant_stem_mesh", m_cylinder, stemMat, {0, len * 0.5f, 0}, {0.012f, len, 0.012f});

        SceneNode* leaf = stem->addChild("plant_leaf");
        leaf->position = Vec3(0.0f, len, 0.0f);
        leaf->rotation.x = 55.0f;
        Material leafMat = makeMat(greens[i % 3], 0.35f, 32.0f);
        leafMat.effect = Effect::Sway;
        leaf->addMesh("plant_leaf_mesh", m_sphere, leafMat, {0, 0.13f, 0}, {0.17f, 0.28f, 0.015f});
    }
}

void DeskScene::buildPC()
{
    // Case with a glass side panel facing the room; interior: motherboard, GPU,
    // RAM and three RGB fans whose rotors spin (hierarchy fan -> rotor -> blades).
    m_pc = m_root->addChild("pc");
    m_pc->position = Vec3(1.22f, 0.0f, -2.08f);

    const float W = 0.45f, H = 0.48f, D = 0.22f;   // x, y, z
    Material body = makeMat({0.2f, 0.2f, 0.22f}, 0.5f, 64.0f);
    m_pc->addMesh("pc_back", m_cube, body, {0, H * 0.5f, -D * 0.5f + 0.005f}, {W, H, 0.01f});
    m_pc->addMesh("pc_top", m_cube, body, {0, H - 0.005f, 0}, {W, 0.01f, D});
    m_pc->addMesh("pc_bottom", m_cube, body, {0, 0.025f, 0}, {W, 0.05f, D});
    m_pc->addMesh("pc_front", m_cube, body, {-W * 0.5f + 0.01f, H * 0.5f, 0}, {0.02f, H, D});
    m_pc->addMesh("pc_rear", m_cube, body, {W * 0.5f - 0.005f, H * 0.5f, 0}, {0.01f, H, D});
    for (float x : {-0.18f, 0.18f})
        m_pc->addMesh("pc_foot", m_cube, body, {x, 0.005f, 0}, {0.05f, 0.01f, D});

    m_pc->addMesh("pc_motherboard", m_cube, makeMat({0.23f, 0.28f, 0.26f}, 0.3f, 32.0f), {0.03f, 0.26f, -D * 0.5f + 0.015f}, {0.3f, 0.32f, 0.01f});

    Material rgbMat = makeMat({0.17f, 0.17f, 0.17f});
    rgbMat.effect = Effect::RgbStrip;

    SceneNode* gpu = m_pc->addChild("pc_gpu");
    gpu->position = Vec3(0.02f, 0.15f, 0.0f);
    gpu->addMesh("gpu_body", m_cube, makeMat({0.38f, 0.38f, 0.4f}, 0.7f, 96.0f), {0, 0, 0}, {0.30f, 0.05f, 0.13f});
    gpu->addMesh("gpu_rgb", m_cube, rgbMat, {0, 0.0f, 0.066f}, {0.26f, 0.008f, 0.002f});

    for (int i = 0; i < 4; ++i)
        m_pc->addMesh("pc_ram", m_cube, rgbMat, {0.10f + i * 0.012f, 0.30f, -0.07f}, {0.005f, 0.08f, 0.03f});

    Material frameMat = makeMat({0.26f, 0.26f, 0.27f}, 0.3f, 32.0f);
    Material blade = makeMat({0.28f, 0.28f, 0.3f}, 0.3f, 32.0f);
    for (int f = 0; f < 3; ++f) {
        SceneNode* fan = m_pc->addChild("pc_fan" + std::to_string(f));
        fan->position = Vec3(-0.14f + f * 0.14f, 0.38f, -D * 0.5f + 0.03f);
        fan->addMesh("fan_frame", m_cube, frameMat, {0, 0, -0.008f}, {0.125f, 0.125f, 0.012f});
        SceneNode* ring = fan->addMesh("fan_ring", m_fanRing, rgbMat, {0, 0, 0.0f}, Vec3(1.0f));
        ring->material.rgbPhase = f * 0.1f;

        SceneNode* rotor = fan->addChild("fan_rotor");
        rotor->addMesh("fan_hub", m_cylinder, frameMat, {0, 0, 0}, {0.03f, 0.012f, 0.03f})->rotation.x = 90.0f;
        for (int b = 0; b < 7; ++b) {
            SceneNode* bl = rotor->addChild("fan_blade");
            bl->rotation.z = b * (360.0f / 7.0f);
            bl->addMesh("fan_blade_mesh", m_cube, blade, {0.03f, 0, 0}, {0.042f, 0.016f, 0.003f})->rotation.x = 25.0f;
        }
        m_fanRotors.push_back(rotor);
    }

    // Tempered glass side panel (transparent pass).
    Material glass = makeMat({0.58f, 0.6f, 0.62f}, 1.5f, 256.0f);
    glass.alpha = 0.18f;
    // 2 mm in front of the case, not in the same plane as its front edges (no z-fighting).
    m_pc->addMesh("pc_glass", m_quad, glass, {0, H * 0.5f, D * 0.5f + 0.002f}, {W - 0.01f, H - 0.01f, 1.0f});
}

void DeskScene::buildSecurityCamera()
{
    // Wall mount -> pan (animated) -> tilt -> body/lens. The feed camera uses the
    // tilt node's world matrix: render-to-texture from a second point of view.
    m_camMount = m_root->addChild("security_camera");
    m_camMount->position = Vec3(2.9f, 2.72f, -2.4f);
    Material white = makeMat({0.93f, 0.93f, 0.93f}, 0.5f, 64.0f);
    m_camMount->addMesh("cam_bracket", m_cube, white, {0.04f, 0.0f, -0.04f}, {0.08f, 0.08f, 0.08f});

    m_camPan = m_camMount->addChild("cam_pan");
    m_camPan->addMesh("cam_neck", m_cylinder, white, {0, -0.03f, 0}, {0.03f, 0.06f, 0.03f});

    m_camTilt = m_camPan->addChild("cam_tilt");
    m_camTilt->position = Vec3(0.0f, -0.07f, 0.0f);
    m_camTilt->rotation.x = 28.0f;   // looks down at the desk
    m_camTilt->addMesh("cam_body", m_cube, white, {0, 0, 0.04f}, {0.07f, 0.065f, 0.15f});
    SceneNode* lens = m_camTilt->addMesh("cam_lens", m_cylinder, makeMat({0.17f, 0.17f, 0.2f}, 1.0f, 200.0f),
                                         {0, 0, 0.12f}, {0.045f, 0.02f, 0.045f});
    lens->rotation.x = 90.0f;

    Material led;
    led.effect = Effect::Unlit;
    m_camLed = m_camTilt->addMesh("cam_led", m_sphere, led, {0.022f, 0.024f, 0.116f}, Vec3(0.008f));
}
