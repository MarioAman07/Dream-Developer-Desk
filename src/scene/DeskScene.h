// DeskScene.h - the whole "Dream Developer Desk" scene: builds the room and all
// objects and animates them.
#pragma once

#include "scene/SceneNode.h"

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

    SceneNode& root() { return *m_root; }
    const SceneNode& root() const { return *m_root; }
    float time() const { return m_time; }

private:

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

    float m_time = 0.0f;
    int m_objectCount = 0;
};
