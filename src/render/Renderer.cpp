#include "render/Renderer.h"

#include "core/Paths.h"
#include "gfx/Mesh.h"
#include "gfx/Texture.h"
#include "scene/DeskScene.h"

#include <algorithm>
#include <cmath>

bool Renderer::init()
{
    bool ok = m_sceneShader.load(paths::shader("scene.vert"), paths::shader("scene.frag"));
    ok = m_skyShader.load(paths::shader("sky.vert"), paths::shader("sky.frag")) && ok;
    return ok;
}

// Walks the scene tree and collects the visible nodes with a mesh.
void Renderer::gather(const SceneNode& node, const Vec3& eye, const SceneNode* sky)
{
    if (!node.visible) return;
    // Cut-away: skip the whole wall (and its children) if the camera is on its
    // outer side, i.e. the eye is behind the plane of the wall.
    if (node.cutaway && dot(eye - node.cutawayPoint, node.cutawayNormal) < 0.0f) return;

    if (&node == sky) {
        m_skyVisible = DeskScene::isInsideRoom(eye);   // from outside the room the sky quad would float in the void
    } else if (node.mesh) {
        m_opaque.push_back(&node);
    }
    for (const auto& c : node.children) gather(*c, eye, sky);
}

void Renderer::setFrameUniforms(const DeskScene& scene, const RenderView& v) const
{
    const Shader& s = m_sceneShader;
    const SceneSettings& st = scene.settings;
    s.use();
    s.setMat4("uView", v.view);
    s.setMat4("uProjection", v.projection);
    s.setVec3("uViewPos", v.eye);
    s.setFloat("uTime", scene.time());
    s.setInt("uRgbMode", st.rgbMode);
    s.setFloat("uRgbSpeed", st.rgbSpeed);
    s.setVec3("uRgbStatic", st.rgbStatic);
    s.setFloat("uRgbLevel", scene.env().rgb);
    s.setFloat("uScreenBrightness", scene.env().screen);
    s.setInt("uTexture", 0);   // the sampler reads texture unit 0
    s.setVec3("uAmbient", m_lights.ambient);
    s.setVec3("uDeskMin", m_lights.deskMin);
    s.setVec3("uDeskMax", m_lights.deskMax);

    int np = std::min<int>(static_cast<int>(m_lights.points.size()), kMaxPointLights);
    s.setInt("uPointCount", np);
    for (int i = 0; i < np; ++i) {
        const PointLight& l = m_lights.points[i];
        std::string b = "uPoints[" + std::to_string(i) + "].";
        s.setVec3(b + "position", l.position);
        s.setVec3(b + "color", l.color * l.intensity);
        s.setFloat(b + "range", l.range);
        s.setBool(b + "deskShadow", l.deskShadow);
    }

    int ns = std::min<int>(static_cast<int>(m_lights.spots.size()), kMaxSpotLights);
    s.setInt("uSpotCount", ns);
    for (int i = 0; i < ns; ++i) {
        const SpotLight& l = m_lights.spots[i];
        std::string b = "uSpots[" + std::to_string(i) + "].";
        s.setVec3(b + "position", l.position);
        s.setVec3(b + "direction", normalize(l.direction));
        s.setVec3(b + "color", l.color * l.intensity);
        s.setFloat(b + "range", l.range);
        s.setFloat(b + "cutoffCos", std::cos(radians(l.cutoffDeg)));
        s.setFloat(b + "exponent", l.exponent);
    }
}

void Renderer::setMaterial(const Material& m, float time) const
{
    const Shader& s = m_sceneShader;
    s.setVec3("uDiffuse", m.albedo);
    s.setFloat("uSpecular", m.specular);
    s.setFloat("uShininess", m.shininess);
    s.setFloat("uAlpha", m.alpha);
    s.setInt("uEffect", static_cast<int>(m.effect));
    s.setFloat("uRgbPhase", m.rgbPhase);
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

void Renderer::drawSky(const DeskScene& scene, const RenderView& v, const SceneNode& sky) const
{
    const Environment& e = scene.env();
    const Shader& s = m_skyShader;
    s.use();
    s.setMat4("uModel", sky.meshWorld());
    s.setMat4("uView", v.view);
    s.setMat4("uProjection", v.projection);
    s.setVec3("uViewPos", v.eye);
    s.setVec3("uSunDir", e.sunDir);
    s.setVec3("uSunColor", e.sunDiscColor);
    s.setVec3("uZenith", e.skyZenith);
    s.setVec3("uHorizon", e.skyHorizon);
    s.setFloat("uStars", e.stars);
    s.setFloat("uDay", e.day);
    s.setFloat("uTime", scene.time());
    sky.mesh->draw();
}

void Renderer::render(const DeskScene& scene, const RenderView& v)
{
    scene.collectLights(m_lights);

    m_opaque.clear();
    m_skyVisible = false;
    gather(scene.root(), v.eye, scene.skyNode());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    // 1) Opaque objects
    setFrameUniforms(scene, v);
    for (const SceneNode* n : m_opaque) drawNode(*n, scene.time());

    // 2) Sky behind the window
    if (m_skyVisible) drawSky(scene, v, *scene.skyNode());
}
