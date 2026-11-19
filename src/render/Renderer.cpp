#include "render/Renderer.h"

#include "core/Paths.h"
#include "gfx/Mesh.h"
#include "gfx/Texture.h"
#include "scene/DeskScene.h"

#include <cmath>

bool Renderer::init()
{
    bool ok = m_sceneShader.load(paths::shader("scene.vert"), paths::shader("scene.frag"));
    return ok;
}

// Walks the scene tree and collects the visible nodes with a mesh.
void Renderer::gather(const SceneNode& node, const Vec3& eye)
{
    if (!node.visible) return;
    // Cut-away: skip the whole wall (and its children) if the camera is on its
    // outer side, i.e. the eye is behind the plane of the wall.
    if (node.cutaway && dot(eye - node.cutawayPoint, node.cutawayNormal) < 0.0f) return;

    if (node.mesh) {
        m_opaque.push_back(&node);
    }
    for (const auto& c : node.children) gather(*c, eye);
}

void Renderer::setFrameUniforms(const DeskScene& scene, const RenderView& v) const
{
    const Shader& s = m_sceneShader;
    s.use();
    s.setMat4("uView", v.view);
    s.setMat4("uProjection", v.projection);
    s.setVec3("uViewPos", v.eye);
    s.setFloat("uTime", scene.time());
    s.setFloat("uScreenBrightness", 1.0f);
    s.setInt("uTexture", 0);   // the sampler reads texture unit 0
}

void Renderer::setMaterial(const Material& m, float time) const
{
    const Shader& s = m_sceneShader;
    s.setVec3("uDiffuse", m.albedo);
    s.setFloat("uAlpha", m.alpha);
    s.setInt("uEffect", static_cast<int>(m.effect));
    s.setBool("uRotateUV", m.rotateUV);
    // Scrolling UVs (code on the screens). Only the fractional part is sent, so
    // the numbers stay small (the texture repeats anyway).
    Vec2 offset = m.uvScrollSpeed * time;
    s.setVec2("uUVOffset", Vec2(offset.x - std::floor(offset.x), offset.y - std::floor(offset.y)));

    glActiveTexture(GL_TEXTURE0);
    if (m.texture) glBindTexture(GL_TEXTURE_2D, m.texture->id());
    else glBindTexture(GL_TEXTURE_2D, 0);
    s.setBool("uUseTexture", m.texture != nullptr);
}

void Renderer::drawNode(const SceneNode& node, float time) const
{
    const Mat4& model = node.meshWorld();
    m_sceneShader.setMat4("uModel", model);
    m_sceneShader.setMat3("uNormalMatrix", normalMatrix(model));
    setMaterial(node.material, time);
    node.mesh->draw();
}

void Renderer::render(const DeskScene& scene, const RenderView& v)
{
    m_opaque.clear();
    gather(scene.root(), v.eye);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    // 1) Opaque objects
    setFrameUniforms(scene, v);
    for (const SceneNode* n : m_opaque) drawNode(*n, scene.time());
}
