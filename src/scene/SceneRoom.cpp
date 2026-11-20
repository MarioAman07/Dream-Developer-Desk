// SceneRoom.cpp - floor, walls, window, ceiling pendant, wall shelf and poster.
// Room interior: x -3..3, y 0..3, z -2.5..2.5 (metres). The desk stands at the back wall.
#include "scene/DeskScene.h"

#include "gfx/Mesh.h"

namespace {
Material makeMat(const Vec3& albedo, float spec = 0.15f, float shin = 16.0f)
{
    Material m;
    m.albedo = albedo;
    m.specular = spec;
    m.shininess = shin;
    return m;
}

// A wall group: hidden when the camera is outside it (cut-away view).
SceneNode* wallGroup(SceneNode* root, const char* name, const Vec3& point, const Vec3& inwardNormal)
{
    SceneNode* g = root->addChild(name);
    g->cutaway = true;
    g->cutawayPoint = point;
    g->cutawayNormal = inwardNormal;
    return g;
}
} // namespace

void DeskScene::buildRoom()
{
    const float t = 0.1f;   // wall thickness

    Material floorMat = makeMat(Vec3(1.0f), 0.35f, 48.0f);
    floorMat.texture = texture("floor_wood");
    m_root->addMesh("floor", makeBox({6.2f, t, 5.2f}, 0.45f), floorMat, {0.0f, -t * 0.5f, 0.0f}, Vec3(1.0f));

    Material wallMat = makeMat(settings.wallColor, 0.05f, 8.0f);
    wallMat.texture = texture("wall");

    // Back wall (behind the desk)
    SceneNode* back = wallGroup(m_root.get(), "wall_back", {0, 0, -2.5f}, {0, 0, 1});
    m_walls.push_back(back->addMesh("wall_back_mesh", makeBox({6.2f, 3.0f, t}, 0.5f), wallMat, {0, 1.5f, -2.5f - t * 0.5f}, Vec3(1)));

    // Front wall
    SceneNode* front = wallGroup(m_root.get(), "wall_front", {0, 0, 2.5f}, {0, 0, -1});
    m_walls.push_back(front->addMesh("wall_front_mesh", makeBox({6.2f, 3.0f, t}, 0.5f), wallMat, {0, 1.5f, 2.5f + t * 0.5f}, Vec3(1)));

    // Right wall
    SceneNode* right = wallGroup(m_root.get(), "wall_right", {3.0f, 0, 0}, {-1, 0, 0});
    m_walls.push_back(right->addMesh("wall_right_mesh", makeBox({t, 3.0f, 5.2f}, 0.5f), wallMat, {3.0f + t * 0.5f, 1.5f, 0}, Vec3(1)));

    // Left wall is built around the window opening (z -1.9..-0.3, y 0.95..2.35).
    SceneNode* left = wallGroup(m_root.get(), "wall_left", {-3.0f, 0, 0}, {1, 0, 0});
    const float x = -3.0f - t * 0.5f;
    m_walls.push_back(left->addMesh("wall_left_low", makeBox({t, 0.95f, 5.2f}, 0.5f), wallMat, {x, 0.475f, 0}, Vec3(1)));
    m_walls.push_back(left->addMesh("wall_left_high", makeBox({t, 0.65f, 5.2f}, 0.5f), wallMat, {x, 2.675f, 0}, Vec3(1)));
    m_walls.push_back(left->addMesh("wall_left_back", makeBox({t, 1.4f, 0.7f}, 0.5f), wallMat, {x, 1.65f, -2.25f}, Vec3(1)));
    m_walls.push_back(left->addMesh("wall_left_front", makeBox({t, 1.4f, 2.9f}, 0.5f), wallMat, {x, 1.65f, 1.15f}, Vec3(1)));
    buildWindow(left);

    // Ceiling
    SceneNode* ceiling = wallGroup(m_root.get(), "ceiling", {0, 3.0f, 0}, {0, -1, 0});
    ceiling->addMesh("ceiling_mesh", m_cube, makeMat({0.93f, 0.93f, 0.93f}, 0.02f), {0, 3.0f + t * 0.5f, 0}, {6.2f, t, 5.2f});

    // Skirting boards add a bit of detail where the walls meet the floor.
    Material skirting = makeMat({0.96f, 0.96f, 0.95f}, 0.3f, 32.0f);
    back->addMesh("skirting_back", m_cube, skirting, {0, 0.04f, -2.49f}, {6.0f, 0.08f, 0.02f});
    right->addMesh("skirting_right", m_cube, skirting, {2.99f, 0.04f, 0}, {0.02f, 0.08f, 5.0f});

    // Rug under the chair (one texture over the whole rug, so a unit cube's 0..1 UVs fit).
    Material rugMat = makeMat(Vec3(1.0f), 0.05f, 8.0f);
    rugMat.texture = texture("rug");
    m_rug = m_root->addMesh("rug", m_cube, rugMat, {0.0f, 0.006f, -0.95f}, {2.2f, 0.012f, 1.6f});

    buildCeilingLamp();
    buildShelfAndPoster();
}

void DeskScene::buildWindow(SceneNode* wall)
{
    const float zc = -1.1f, yc = 1.65f, w = 1.6f, h = 1.4f;
    Material frame = makeMat({0.97f, 0.97f, 0.96f}, 0.4f, 48.0f);

    SceneNode* win = wall->addChild("window");
    win->position = Vec3(-3.0f, yc, zc);
    win->addMesh("frame_top", m_cube, frame, {0, h * 0.5f, 0}, {0.14f, 0.06f, w + 0.06f});
    win->addMesh("frame_bottom", m_cube, frame, {0, -h * 0.5f, 0}, {0.14f, 0.06f, w + 0.06f});
    win->addMesh("frame_left", m_cube, frame, {0, 0, -w * 0.5f}, {0.14f, h, 0.06f});
    win->addMesh("frame_right", m_cube, frame, {0, 0, w * 0.5f}, {0.14f, h, 0.06f});
    win->addMesh("frame_mid", m_cube, frame, {0, 0, 0}, {0.1f, h, 0.04f});
    win->addMesh("sill", m_cube, frame, {0.1f, -h * 0.5f - 0.02f, 0}, {0.26f, 0.03f, w + 0.2f});
}

void DeskScene::buildCeilingLamp()
{
    // Pendant: pivot on the ceiling -> cord -> shade -> bulb.
    m_pendant = m_root->addChild("pendant");
    m_pendant->position = Vec3(0.0f, 3.0f, -1.4f);

    Material cord = makeMat({0.26f, 0.26f, 0.26f}, 0.2f);
    m_pendant->addMesh("pendant_canopy", m_cylinder, cord, {0, -0.01f, 0}, {0.12f, 0.02f, 0.12f});
    m_pendant->addMesh("pendant_cord", m_cylinder, cord, {0, -0.35f, 0}, {0.008f, 0.7f, 0.008f});

    SceneNode* shade = m_pendant->addChild("pendant_shade");
    shade->position = Vec3(0.0f, -0.78f, 0.0f);
    Material shadeMat = makeMat({0.32f, 0.33f, 0.35f}, 0.8f, 64.0f);
    shade->addMesh("pendant_shade_mesh", m_cone, shadeMat, {0, 0, 0}, {0.36f, 0.2f, 0.36f});

    Material bulb;
    bulb.effect = Effect::Unlit;
    m_ceilingBulb = shade->addMesh("pendant_bulb", m_sphere, bulb, {0, -0.1f, 0}, Vec3(0.07f));
}

void DeskScene::buildShelfAndPoster()
{
    // Wall shelf above the monitors.
    m_shelf = m_root->addChild("shelf");
    m_shelf->position = Vec3(-0.55f, 1.78f, -2.38f);
    Material board = makeMat({1.0f, 1.0f, 1.0f}, 0.3f, 32.0f);
    board.texture = texture("desk_wood");
    m_shelf->addMesh("shelf_board", makeBox({1.2f, 0.03f, 0.24f}, 1.0f), board, {0, 0, 0}, Vec3(1.0f));
    Material bracket = makeMat({0.35f, 0.35f, 0.35f}, 0.5f, 64.0f);
    m_shelf->addMesh("shelf_bracket_l", m_cube, bracket, {-0.45f, -0.06f, -0.08f}, {0.02f, 0.1f, 0.08f});
    m_shelf->addMesh("shelf_bracket_r", m_cube, bracket, {0.45f, -0.06f, -0.08f}, {0.02f, 0.1f, 0.08f});

    // Books with different sizes and colours.
    const Vec3 colors[] = {{0.76f, 0.38f, 0.38f}, {0.38f, 0.53f, 0.7f}, {0.93f, 0.82f, 0.48f}, {0.42f, 0.66f, 0.53f},
                                {0.58f, 0.58f, 0.6f}, {0.79f, 0.58f, 0.73f}, {0.95f, 0.95f, 0.93f}, {0.48f, 0.76f, 0.79f}};
    float bx = -0.55f;
    for (int i = 0; i < 8; ++i) {
        float bw = 0.028f + 0.012f * static_cast<float>((i * 7) % 3);
        float bh = 0.20f + 0.03f * static_cast<float>((i * 5) % 4);
        SceneNode* book = m_shelf->addMesh("book" + std::to_string(i), m_cube, makeMat(colors[i], 0.2f, 24.0f),
                                           {bx + bw * 0.5f, 0.015f + bh * 0.5f, 0.0f}, {bw, bh, 0.16f});
        if (i == 7) book->rotation.z = -14.0f;   // the last book leans over
        bx += bw + 0.004f;
    }

    // Rubber duck - every developer's debugging partner.
    SceneNode* duck = m_shelf->addChild("rubber_duck");
    duck->position = Vec3(0.25f, 0.015f, 0.02f);
    duck->rotation.y = -25.0f;
    Material yellow = makeMat({1.0f, 0.91f, 0.35f}, 0.6f, 64.0f);
    duck->addMesh("duck_body", m_sphere, yellow, {0, 0.045f, 0}, {0.11f, 0.08f, 0.13f});
    duck->addMesh("duck_head", m_sphere, yellow, {0, 0.105f, 0.035f}, Vec3(0.07f));
    duck->addMesh("duck_beak", m_sphere, makeMat({1.0f, 0.7f, 0.26f}, 0.5f, 32.0f), {0, 0.1f, 0.075f}, {0.035f, 0.015f, 0.04f});

    // Small cactus in a pot.
    SceneNode* cactus = m_shelf->addChild("cactus");
    cactus->position = Vec3(0.45f, 0.015f, 0.0f);
    cactus->addMesh("cactus_pot", m_cylinder, makeMat({0.93f, 0.93f, 0.91f}, 0.4f, 32.0f), {0, 0.035f, 0}, {0.07f, 0.07f, 0.07f});
    Material green = makeMat({0.46f, 0.7f, 0.48f}, 0.2f, 16.0f);
    green.effect = Effect::Sway;
    cactus->addMesh("cactus_body", m_sphere, green, {0, 0.11f, 0}, {0.05f, 0.12f, 0.05f});
    cactus->addMesh("cactus_arm", m_sphere, green, {0.025f, 0.12f, 0}, {0.025f, 0.05f, 0.025f});

    // Poster in a black frame.
    m_poster = m_root->addChild("poster");
    m_poster->position = Vec3(1.55f, 1.65f, -2.5f);
    m_poster->addMesh("poster_frame", m_cube, makeMat({0.2f, 0.2f, 0.2f}, 0.5f, 64.0f), {0, 0, 0.01f}, {0.56f, 0.76f, 0.02f});
    Material art = makeMat(Vec3(1.0f), 0.25f, 64.0f);
    art.texture = texture("poster");
    m_poster->addMesh("poster_art", m_quad, art, {0, 0, 0.022f}, {0.5f, 0.7f, 1.0f});   // 2 mm in front of the frame
}
