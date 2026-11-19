// SceneNode.h - scene graph. Every node has a local transform that is
// inherited by its children (parent -> child hierarchy, e.g. lamp base -> arm ->
// arm -> head) and an optional mesh with its own mesh-only transform, so that
// scaling a mesh never distorts the children attached to it.
#pragma once

#include "scene/Material.h"

#include "core/Math.h"

#include <memory>
#include <string>
#include <vector>

class Mesh;

class SceneNode {
public:
    explicit SceneNode(std::string name = "node");

    // Creates a child node and returns a pointer to it (the parent owns it).
    SceneNode* addChild(const std::string& name);
    SceneNode* addMesh(const std::string& name, const Mesh* mesh, const Material& material,
                       const Vec3& offset, const Vec3& size);

    // Recomputes world matrices for this node and all children.
    void updateWorld(const Mat4& parentWorld = Mat4());

    Mat4 localMatrix() const;
    const Mat4& world() const { return m_world; }
    const Mat4& meshWorld() const { return m_meshWorld; }
    Vec3 worldPosition() const { return m_world.column(3); }
    Vec3 worldAxis(int i) const { return normalize(m_world.column(i)); }

    // ---- local transform (inherited by children) ----
    std::string name;
    Vec3 position{0.0f};
    Vec3 rotation{0.0f};   // degrees: x = pitch, y = yaw, z = roll (applied Y, X, Z)
    Vec3 scale{1.0f};

    // ---- mesh-only transform (NOT inherited) ----
    const Mesh* mesh = nullptr;
    Material material;
    Vec3 meshOffset{0.0f};
    Vec3 meshScale{1.0f};

    bool visible = true;

    // Cut-away walls: the node is hidden when the camera is on the outer side of
    // the plane (point, inward normal). Lets the orbit camera look in from outside.
    bool cutaway = false;
    Vec3 cutawayPoint{0.0f};
    Vec3 cutawayNormal{0.0f, 0.0f, 1.0f};

    std::vector<std::unique_ptr<SceneNode>> children;

private:
    Mat4 m_world;
    Mat4 m_meshWorld;
};
