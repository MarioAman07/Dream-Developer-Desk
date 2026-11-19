// DeskScene.h - the whole "Dream Developer Desk" scene: builds the room and all
// objects and animates them.
// Building is split over three files: SceneRoom.cpp, SceneDesk.cpp, SceneProps.cpp.
#pragma once

#include "scene/Keyboard.h"
#include "scene/MonitorRig.h"
#include "scene/SceneNode.h"
#include "scene/Settings.h"

#include "core/Math.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Mesh;
class Texture;

class DeskScene {
public:
    DeskScene();
    ~DeskScene();

    bool init();
    void update(float dt, float time);

    // Monitor presets: arrangement + what each screen shows.
    void applyArrangementPreset(Arrangement a);
    void nextArrangement();
    void spinChair(float impulseDegPerSec) { m_chairSpin += impulseDegPerSec; }

    SceneNode& root() { return *m_root; }
    const SceneNode& root() const { return *m_root; }
    // Size of the desk top in metres.
    static constexpr float kDeskWidth = 1.8f;
    static constexpr float kDeskDepth = 0.8f;
    static constexpr float kDeskThickness = 0.04f;

    float time() const { return m_time; }

    SceneSettings settings;

private:
    // SceneRoom.cpp
    void buildRoom();
    void buildWindow(SceneNode* wall);
    void buildCeilingLamp();
    void buildShelfAndPoster();
    // SceneDesk.cpp
    void buildDesk();
    void buildDeskItems();
    void buildLamp();
    // SceneProps.cpp
    void buildChair();
    void buildPlant();
    void buildPC();

    void applySettingsToNodes(float dt);
    const Mesh* makeBox(const Vec3& size, float uvPerMetre);
    const Texture* texture(const std::string& name) const;
    void loadTextures();

    std::unique_ptr<SceneNode> m_root;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

    const Mesh* m_cube = nullptr;
    const Mesh* m_quad = nullptr;
    const Mesh* m_cylinder = nullptr;
    const Mesh* m_sphere = nullptr;
    const Mesh* m_cone = nullptr;
    const Mesh* m_fanRing = nullptr;
    const Mesh* m_mugHandle = nullptr;

    Keyboard m_keyboard;
    MonitorRig m_monitors;

    // Nodes animated or changed every frame
    SceneNode* m_deskTop = nullptr;
    SceneNode* m_pendant = nullptr;
    SceneNode* m_ceilingBulb = nullptr;
    SceneNode* m_lampRoot = nullptr;
    SceneNode* m_lampYaw = nullptr;
    SceneNode* m_lampLower = nullptr;
    SceneNode* m_lampUpper = nullptr;
    SceneNode* m_lampHead = nullptr;
    SceneNode* m_lampBulb = nullptr;
    SceneNode* m_chairRoot = nullptr;
    SceneNode* m_chairSwivel = nullptr;
    SceneNode* m_plant = nullptr;
    SceneNode* m_mug = nullptr;
    SceneNode* m_pc = nullptr;
    SceneNode* m_shelf = nullptr;
    SceneNode* m_poster = nullptr;
    SceneNode* m_rug = nullptr;
    std::vector<SceneNode*> m_fanRotors;
    std::vector<SceneNode*> m_walls;
    std::vector<SceneNode*> m_chairColored;
    std::vector<SceneNode*> m_chairAccent;

    float m_time = 0.0f;
    float m_fanAngle = 0.0f;
    float m_chairAngle = 0.0f;
    float m_chairSpin = 0.0f;
    int m_objectCount = 0;
};
