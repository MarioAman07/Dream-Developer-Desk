// SceneDesk.cpp - the desk, mouse, mug and the
// articulated desk lamp. Everything on the desk is a child of the desk-top node,
// so moving the desk moves all of it.
#include "scene/DeskScene.h"

#include "gfx/Mesh.h"

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

void DeskScene::buildDesk()
{
    const float width = kDeskWidth, depth = kDeskDepth, thick = kDeskThickness, height = 0.75f;

    // Desk-top node sits exactly on the top surface (y = 0.75), centred on the desk.
    m_deskTop = m_root->addChild("desk");
    m_deskTop->position = Vec3(0.0f, height, -2.0f);

    Material wood = makeMat(settings.deskTint, 0.35f, 48.0f);
    wood.texture = texture("desk_wood");
    m_deskTop->addMesh("desk_board", makeBox({width, thick, depth}, 1.0f), wood, {0, -thick * 0.5f, 0}, Vec3(1.0f));

    // Black metal frame: two T-shaped legs and a cross bar.
    Material metal = makeMat({0.26f, 0.26f, 0.27f}, 0.6f, 80.0f);
    for (float side : {-1.0f, 1.0f}) {
        SceneNode* leg = m_deskTop->addChild(side < 0 ? "leg_left" : "leg_right");
        leg->position = Vec3(side * 0.8f, -thick, 0.0f);
        float legH = height - thick;
        leg->addMesh("leg_post", m_cube, metal, {0, -legH * 0.5f, 0}, {0.06f, legH, 0.06f});
        leg->addMesh("leg_foot", m_cube, metal, {0, -legH + 0.015f, 0}, {0.07f, 0.03f, 0.72f});
        leg->addMesh("leg_top", m_cube, metal, {0, -0.015f, 0}, {0.06f, 0.03f, 0.7f});
    }
    m_deskTop->addMesh("desk_crossbar", m_cube, metal, {0, -0.3f, -0.3f}, {1.6f, 0.05f, 0.025f});
}

void DeskScene::buildDeskItems()
{
    // Desk mat under the keyboard and the mouse.
    Material mat = makeMat(Vec3(1.0f), 0.08f, 8.0f);
    mat.texture = texture("deskmat");
    m_deskTop->addMesh("desk_mat", m_cube, mat, {0.06f, 0.0015f, 0.22f}, {0.92f, 0.003f, 0.36f});

    // Mouse: flattened sphere + scroll wheel + RGB logo.
    SceneNode* mouse = m_deskTop->addChild("mouse");
    mouse->position = Vec3(0.30f, 0.003f, 0.25f);
    mouse->rotation.y = -6.0f;
    mouse->addMesh("mouse_body", m_sphere, makeMat({0.26f, 0.26f, 0.28f}, 0.6f, 64.0f), {0, 0.014f, 0}, {0.064f, 0.036f, 0.115f});
    SceneNode* wheel = mouse->addMesh("mouse_wheel", m_cylinder, makeMat({0.48f, 0.48f, 0.5f}, 0.3f), {0, 0.031f, -0.025f}, {0.018f, 0.008f, 0.018f});
    wheel->rotation.z = 90.0f;
    Material logo = makeMat({0.17f, 0.17f, 0.17f});
    mouse->addMesh("mouse_logo", m_sphere, logo, {0, 0.026f, 0.03f}, {0.016f, 0.008f, 0.016f});

    // Coffee mug: body, coffee surface, handle (torus).
    m_mug = m_deskTop->addChild("mug");
    m_mug->position = Vec3(0.62f, 0.0f, 0.22f);
    m_mug->rotation.y = -30.0f;
    Material ceramic = makeMat({0.95f, 0.95f, 0.94f}, 0.8f, 96.0f);
    m_mug->addMesh("mug_body", m_cylinder, ceramic, {0, 0.05f, 0}, {0.08f, 0.1f, 0.08f});
    // Coffee: a thin disc whose top is 1.5 mm above the top of the mug (the mug is a
    // solid cylinder, so anything lower would be hidden inside it).
    m_mug->addMesh("mug_coffee", m_cylinder, makeMat({0.38f, 0.28f, 0.2f}, 0.9f, 128.0f), {0, 0.0995f, 0}, {0.072f, 0.004f, 0.072f});
    m_mug->addMesh("mug_handle", m_mugHandle, ceramic, {0.045f, 0.05f, 0}, Vec3(1.0f));

    // Headphone stand with headphones (torus band + two ear cups).
    SceneNode* stand = m_deskTop->addChild("headphones");
    stand->position = Vec3(0.78f, 0.0f, -0.05f);
    stand->rotation.y = -40.0f;
    Material dark = makeMat({0.32f, 0.32f, 0.33f}, 0.5f, 48.0f);
    stand->addMesh("hp_base", m_cylinder, dark, {0, 0.006f, 0}, {0.1f, 0.012f, 0.1f});
    stand->addMesh("hp_pole", m_cylinder, dark, {0, 0.13f, 0}, {0.016f, 0.25f, 0.016f});
    SceneNode* band = stand->addMesh("hp_band", m_fanRing, dark, {0, 0.22f, 0}, {1.6f, 1.6f, 3.0f});
    band->rotation.y = 90.0f;
    Material cup = makeMat({0.38f, 0.38f, 0.41f}, 0.4f, 32.0f);
    for (float s : {-1.0f, 1.0f}) {
        SceneNode* c = stand->addMesh("hp_cup", m_cylinder, cup, {0, 0.17f, s * 0.08f}, {0.07f, 0.03f, 0.08f});
        c->rotation.x = 90.0f;
    }
}

void DeskScene::buildLamp()
{
    // Articulated desk lamp - 5-level hierarchy:
    // base -> yaw joint -> lower arm -> upper arm -> head (-> bulb + spot light)
    Material metal = makeMat({0.88f, 0.88f, 0.89f}, 0.9f, 120.0f);
    Material joint = makeMat({0.32f, 0.32f, 0.32f}, 0.5f, 48.0f);

    m_lampRoot = m_deskTop->addChild("lamp");
    m_lampRoot->position = Vec3(-0.78f, 0.0f, 0.28f);
    m_lampRoot->addMesh("lamp_base", m_cylinder, metal, {0, 0.012f, 0}, {0.15f, 0.024f, 0.15f});

    m_lampYaw = m_lampRoot->addChild("lamp_yaw");
    m_lampYaw->position = Vec3(0.0f, 0.024f, 0.0f);
    m_lampYaw->addMesh("lamp_yaw_joint", m_sphere, joint, {0, 0.01f, 0}, Vec3(0.035f));

    const float lowerLen = 0.34f, upperLen = 0.32f;
    m_lampLower = m_lampYaw->addChild("lamp_lower_arm");
    m_lampLower->position = Vec3(0.0f, 0.012f, 0.0f);
    m_lampLower->addMesh("lamp_lower_mesh", m_cylinder, metal, {0, lowerLen * 0.5f, 0}, {0.016f, lowerLen, 0.016f});
    m_lampLower->addMesh("lamp_spring", m_cylinder, joint, {0, lowerLen * 0.5f, -0.02f}, {0.006f, lowerLen * 0.8f, 0.006f});

    m_lampUpper = m_lampLower->addChild("lamp_upper_arm");
    m_lampUpper->position = Vec3(0.0f, lowerLen, 0.0f);
    m_lampUpper->addMesh("lamp_elbow", m_sphere, joint, {0, 0, 0}, Vec3(0.03f));
    m_lampUpper->addMesh("lamp_upper_mesh", m_cylinder, metal, {0, upperLen * 0.5f, 0}, {0.014f, upperLen, 0.014f});

    m_lampHead = m_lampUpper->addChild("lamp_head");
    m_lampHead->position = Vec3(0.0f, upperLen, 0.0f);
    m_lampHead->addMesh("lamp_head_joint", m_sphere, joint, {0, 0, 0}, Vec3(0.028f));
    // Shade: cone whose wide end points along the head's -Y axis (the light direction).
    m_lampHead->addMesh("lamp_shade", m_cone, metal, {0, -0.06f, 0}, {0.13f, 0.12f, 0.13f});

    Material bulb;
    m_lampBulb = m_lampHead->addMesh("lamp_bulb", m_sphere, bulb, {0, -0.115f, 0}, Vec3(0.045f));
}
