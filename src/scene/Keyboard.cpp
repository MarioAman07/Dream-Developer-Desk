#include "scene/Keyboard.h"

#include "gfx/Primitives.h"
#include "scene/SceneNode.h"

#include <cmath>
#include <vector>

namespace {
// Key widths in units; negative numbers are gaps. Standard 75%-style layout.
const std::vector<std::vector<float>> kLayout = {
    {1, -1, 1, 1, 1, 1, -0.5f, 1, 1, 1, 1, -0.5f, 1, 1, 1, 1},          // Esc, F1..F12
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},                         // `1234567890-= Backspace
    {1.5f, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.5f},                   // Tab QWERTY... brackets, backslash
    {1.75f, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2.25f},                    // Caps ASDF... Enter
    {2.25f, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2.75f},                       // Shift ZXCV... Shift
    {1.25f, 1.25f, 1.25f, 6.25f, 1.25f, 1.25f, 1.25f, 1.25f},           // Ctrl Win Alt Space ...
};
constexpr float kRowsWidthUnits = 15.0f;
} // namespace

Keyboard::Keyboard() = default;
Keyboard::~Keyboard() = default;

void Keyboard::build(SceneNode* parent, const Mesh* cube, const Vec3& position)
{
    const float u = kUnit;
    const float width = kRowsWidthUnits * u + 0.014f;
    const float depth = 6.25f * u + 0.014f;

    m_node = parent->addChild("keyboard");
    m_node->position = position;
    m_node->rotation = Vec3(3.0f, 0.0f, 0.0f);   // back edge slightly raised

    Material plate;
    plate.albedo = Vec3(0.28f, 0.28f, 0.3f);
    plate.specular = 0.5f;
    plate.shininess = 64.0f;
    m_node->addMesh("keyboard_plate", cube, plate, {0.0f, kPlateHeight * 0.5f, 0.0f}, {width, kPlateHeight, depth});

    // Thin RGB light bar along the front edge of the plate.
    Material bar;
    bar.albedo = Vec3(0.17f);
    bar.effect = Effect::RgbStrip;
    bar.rgbPhase = 0.3f;
    m_node->addMesh("keyboard_lightbar", cube, bar, {0.0f, 0.004f, depth * 0.5f + 0.0008f}, {width - 0.01f, 0.003f, 0.0016f});

    // All keycaps -> one vertex array and one index array.
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
    const float keyY = kPlateHeight + kKeyHeight * 0.5f;
    float z = -depth * 0.5f + 0.007f + u * 0.5f;
    for (size_t row = 0; row < kLayout.size(); ++row) {
        float x = -kRowsWidthUnits * u * 0.5f;
        for (float w : kLayout[row]) {
            float keyW = std::abs(w) * u;
            if (w > 0.0f) {
                float centreX = x + keyW * 0.5f;
                // uv.x = phase of the RGB wave (travels from left to right), uv.y = key number
                Vec2 keyData(centreX * 3.0f - static_cast<float>(row) * 0.04f, static_cast<float>(m_keyCount));
                prim::appendBox(vertices, indices, {centreX, keyY, z}, {keyW - kKeyGap, kKeyHeight, u - kKeyGap}, keyData);
                ++m_keyCount;
            }
            x += keyW;
        }
        z += u * (row == 0 ? 1.25f : 1.0f);   // small gap under the function row
    }
    m_keyMesh = std::make_unique<Mesh>(vertices, indices);

    Material keys;
    keys.albedo = Vec3(0.24f, 0.24f, 0.26f);
    keys.specular = 0.35f;
    keys.shininess = 40.0f;
    keys.effect = Effect::KeyCap;
    m_node->addMesh("keycaps", m_keyMesh.get(), keys, Vec3(0.0f), Vec3(1.0f));
}
