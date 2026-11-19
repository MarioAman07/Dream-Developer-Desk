// Keyboard.h - mechanical keyboard with a real key layout. All keycaps are
// merged into ONE static mesh at startup (batching), so the whole keyboard is a
// single draw call. Every vertex of a key stores in its uv attribute the phase
// of the RGB wave (from the key position) and the number of the key; the
// shaders use them for the RGB colour and the "ghost typing" animation.
#pragma once

#include "core/Math.h"

#include <memory>

class Mesh;
class SceneNode;

class Keyboard {
public:
    Keyboard();
    ~Keyboard();

    // Builds the plate, the light bar and the keycap mesh under `parent`.
    void build(SceneNode* parent, const Mesh* cube, const Vec3& position);

    SceneNode* node() const { return m_node; }
    int keyCount() const { return m_keyCount; }

    static constexpr float kUnit = 0.0185f;       // 1u key pitch in metres
    static constexpr float kPlateHeight = 0.018f;
    static constexpr float kKeyHeight = 0.009f;
    static constexpr float kKeyGap = 0.0028f;

private:
    SceneNode* m_node = nullptr;
    std::unique_ptr<Mesh> m_keyMesh;
    int m_keyCount = 0;
};
