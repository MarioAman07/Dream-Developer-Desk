// Renderer.h - draws a DeskScene from one point of view. Used twice per frame
// when the security camera feed is visible: once into the framebuffer, once to
// the window.
// Opaque objects are drawn first; transparent ones (glass) after them, sorted
// back to front and without depth writes (lecture "Pixels and Buffers", blending).
#pragma once

#include "gfx/Shader.h"
#include "scene/Lights.h"

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
    void gather(const SceneNode& node, const Vec3& eye, const SceneNode* sky);
    void setFrameUniforms(const DeskScene& scene, const RenderView& v) const;
    void setMaterial(const Material& m, float time) const;
    void drawNode(const SceneNode& node, float time) const;
    void drawSky(const DeskScene& scene, const RenderView& v, const SceneNode& sky) const;

    Shader m_sceneShader;   // every object: scene.vert + scene.frag
    Shader m_skyShader;     // the sky behind the window: sky.vert + sky.frag

    LightSet m_lights;
    std::vector<const SceneNode*> m_opaque;
    std::vector<const SceneNode*> m_transparent;
    bool m_skyVisible = false;
};
