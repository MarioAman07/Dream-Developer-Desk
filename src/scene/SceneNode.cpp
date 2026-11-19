#include "scene/SceneNode.h"

namespace {
// Instance transformation M = T * R * S (lecture "Transformations"): scale first,
// then rotate (yaw around Y, pitch around X, roll around Z), then translate.
Mat4 trs(const Vec3& t, const Vec3& rDeg, const Vec3& s)
{
    return translation(t) * rotationY(rDeg.y) * rotationX(rDeg.x) * rotationZ(rDeg.z) * scaling(s);
}
} // namespace

SceneNode::SceneNode(std::string n) : name(std::move(n)) {}

SceneNode* SceneNode::addChild(const std::string& childName)
{
    children.push_back(std::make_unique<SceneNode>(childName));
    return children.back().get();
}

SceneNode* SceneNode::addMesh(const std::string& childName, const Mesh* m, const Material& mat,
                              const Vec3& offset, const Vec3& size)
{
    SceneNode* node = addChild(childName);
    node->mesh = m;
    node->material = mat;
    node->position = offset;
    node->meshScale = size;
    return node;
}

Mat4 SceneNode::localMatrix() const
{
    return trs(position, rotation, scale);
}

void SceneNode::updateWorld(const Mat4& parentWorld)
{
    // world = parent * local  (this is the whole hierarchy in one line)
    m_world = parentWorld * localMatrix();
    m_meshWorld = m_world * translation(meshOffset) * scaling(meshScale);
    for (auto& c : children) c->updateWorld(m_world);
}
