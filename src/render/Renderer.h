// Renderer.h - draws a DeskScene from one point of view.
#pragma once

#include "gfx/Shader.h"

#include <string>
#include <vector>

class DeskScene;
class SceneNode;
struct Material;

struct RenderView {
    Mat4 view;
    Mat4 projection;
    Vec3 eye;
};

class Renderer {
public:
    bool init();
    void render(const DeskScene& scene, const RenderView& view);

private:
    void gather(const SceneNode& node, const Vec3& eye);
    void setFrameUniforms(const DeskScene& scene, const RenderView& v) const;
    void setMaterial(const Material& m, float time) const;
    void drawNode(const SceneNode& node, float time) const;

    Shader m_sceneShader;   // every object: scene.vert + scene.frag

    std::vector<const SceneNode*> m_opaque;
};
