#include "gfx/Primitives.h"

#include <cmath>

namespace {

// Adds one rectangular face: corner + u*du + v*dv, all four vertices share `normal`.
void addFace(std::vector<Vertex>& v, std::vector<unsigned>& idx, const Vec3& corner,
             const Vec3& du, const Vec3& dv, const Vec3& normal, const Vec2& uvSize)
{
    unsigned base = static_cast<unsigned>(v.size());
    v.push_back({corner, normal, {0.0f, 0.0f}});
    v.push_back({corner + du, normal, {uvSize.x, 0.0f}});
    v.push_back({corner + du + dv, normal, {uvSize.x, uvSize.y}});
    v.push_back({corner + dv, normal, {0.0f, uvSize.y}});
    idx.insert(idx.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
}

// The 6 faces of a box of size s centred at the origin.
void addBoxFaces(std::vector<Vertex>& v, std::vector<unsigned>& idx, const Vec3& s, const Vec3& uvDensity)
{
    Vec3 h = s * 0.5f;
    // Face winding is counter-clockwise when seen from outside.
    addFace(v, idx, {-h.x, -h.y, h.z}, {s.x, 0, 0}, {0, s.y, 0}, {0, 0, 1}, {s.x * uvDensity.x, s.y * uvDensity.y});   // +Z
    addFace(v, idx, {h.x, -h.y, -h.z}, {-s.x, 0, 0}, {0, s.y, 0}, {0, 0, -1}, {s.x * uvDensity.x, s.y * uvDensity.y}); // -Z
    addFace(v, idx, {h.x, -h.y, h.z}, {0, 0, -s.z}, {0, s.y, 0}, {1, 0, 0}, {s.z * uvDensity.z, s.y * uvDensity.y});   // +X
    addFace(v, idx, {-h.x, -h.y, -h.z}, {0, 0, s.z}, {0, s.y, 0}, {-1, 0, 0}, {s.z * uvDensity.z, s.y * uvDensity.y}); // -X
    addFace(v, idx, {-h.x, h.y, h.z}, {s.x, 0, 0}, {0, 0, -s.z}, {0, 1, 0}, {s.x * uvDensity.x, s.z * uvDensity.z});   // +Y
    addFace(v, idx, {-h.x, -h.y, -h.z}, {s.x, 0, 0}, {0, 0, s.z}, {0, -1, 0}, {s.x * uvDensity.x, s.z * uvDensity.z}); // -Y
}

std::unique_ptr<Mesh> makeBox(const Vec3& s, const Vec3& uvDensity)
{
    std::vector<Vertex> v;
    std::vector<unsigned> idx;
    addBoxFaces(v, idx, s, uvDensity);
    return std::make_unique<Mesh>(v, idx);
}

} // namespace

namespace prim {

std::unique_ptr<Mesh> cube()
{
    // uvDensity = 1 on a unit cube gives 0..1 UVs on every face.
    return makeBox(Vec3(1.0f), Vec3(1.0f));
}

std::unique_ptr<Mesh> box(const Vec3& size, float uvPerMetre)
{
    return makeBox(size, Vec3(uvPerMetre));
}

void appendBox(std::vector<Vertex>& v, std::vector<unsigned>& idx, const Vec3& centre, const Vec3& size, const Vec2& uv)
{
    size_t first = v.size();
    addBoxFaces(v, idx, size, Vec3(1.0f));
    for (size_t i = first; i < v.size(); ++i) {
        v[i].position += centre;
        v[i].uv = uv;
    }
}

std::unique_ptr<Mesh> quad()
{
    std::vector<Vertex> v;
    std::vector<unsigned> idx;
    addFace(v, idx, {-0.5f, -0.5f, 0.0f}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1});
    return std::make_unique<Mesh>(v, idx);
}

std::unique_ptr<Mesh> cylinder(int segments)
{
    std::vector<Vertex> v;
    std::vector<unsigned> idx;

    // Side: duplicate the seam column so UVs wrap correctly.
    for (int i = 0; i <= segments; ++i) {
        float t = static_cast<float>(i) / segments;
        float a = t * 2.0f * PI;
        Vec3 n(std::cos(a), 0.0f, std::sin(a));
        v.push_back({Vec3(n.x * 0.5f, -0.5f, n.z * 0.5f), n, {t, 0.0f}});
        v.push_back({Vec3(n.x * 0.5f, 0.5f, n.z * 0.5f), n, {t, 1.0f}});
    }
    for (int i = 0; i < segments; ++i) {
        unsigned b = i * 2;
        idx.insert(idx.end(), {b, b + 1, b + 3, b, b + 3, b + 2});
    }

    // Caps (triangle fans around a centre vertex).
    for (int cap = 0; cap < 2; ++cap) {
        float y = cap == 0 ? 0.5f : -0.5f;
        Vec3 n(0.0f, cap == 0 ? 1.0f : -1.0f, 0.0f);
        unsigned centre = static_cast<unsigned>(v.size());
        v.push_back({Vec3(0.0f, y, 0.0f), n, {0.5f, 0.5f}});
        for (int i = 0; i <= segments; ++i) {
            float a = static_cast<float>(i) / segments * 2.0f * PI;
            float c = std::cos(a), s = std::sin(a);
            v.push_back({Vec3(c * 0.5f, y, s * 0.5f), n, {0.5f + c * 0.5f, 0.5f + s * 0.5f}});
        }
        for (int i = 0; i < segments; ++i) {
            unsigned a = centre + 1 + i, b = centre + 2 + i;
            if (cap == 0) idx.insert(idx.end(), {centre, b, a});
            else          idx.insert(idx.end(), {centre, a, b});
        }
    }
    return std::make_unique<Mesh>(v, idx);
}

std::unique_ptr<Mesh> sphere(int stacks, int slices)
{
    std::vector<Vertex> v;
    std::vector<unsigned> idx;
    for (int i = 0; i <= stacks; ++i) {
        float vt = static_cast<float>(i) / stacks;
        float phi = vt * PI;                        // 0 at the top, pi at the bottom
        for (int j = 0; j <= slices; ++j) {
            float ut = static_cast<float>(j) / slices;
            float theta = ut * 2.0f * PI;
            Vec3 n(std::sin(phi) * std::cos(theta), std::cos(phi), std::sin(phi) * std::sin(theta));
            v.push_back({n * 0.5f, n, {ut, 1.0f - vt}});
        }
    }
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            unsigned a = i * (slices + 1) + j;
            unsigned b = a + slices + 1;
            idx.insert(idx.end(), {a, a + 1, b, b, a + 1, b + 1});
        }
    }
    return std::make_unique<Mesh>(v, idx);
}

std::unique_ptr<Mesh> cone(int segments)
{
    std::vector<Vertex> v;
    std::vector<unsigned> idx;
    // Side normal of a cone with radius 0.5 and height 1: slope = 0.5 / 1.
    const float ny = 0.5f / std::sqrt(1.0f + 0.25f);
    const float nr = 1.0f / std::sqrt(1.0f + 0.25f);
    for (int i = 0; i <= segments; ++i) {
        float t = static_cast<float>(i) / segments;
        float a = t * 2.0f * PI;
        Vec3 n(std::cos(a) * nr, ny, std::sin(a) * nr);
        v.push_back({Vec3(std::cos(a) * 0.5f, -0.5f, std::sin(a) * 0.5f), n, {t, 0.0f}});
        v.push_back({Vec3(0.0f, 0.5f, 0.0f), n, {t, 1.0f}});
    }
    for (int i = 0; i < segments; ++i) {
        unsigned b = i * 2;
        idx.insert(idx.end(), {b, b + 1, b + 2});
    }
    unsigned centre = static_cast<unsigned>(v.size());
    v.push_back({Vec3(0.0f, -0.5f, 0.0f), {0, -1, 0}, {0.5f, 0.5f}});
    for (int i = 0; i <= segments; ++i) {
        float a = static_cast<float>(i) / segments * 2.0f * PI;
        v.push_back({Vec3(std::cos(a) * 0.5f, -0.5f, std::sin(a) * 0.5f), {0, -1, 0},
                     {0.5f + std::cos(a) * 0.5f, 0.5f + std::sin(a) * 0.5f}});
    }
    for (int i = 0; i < segments; ++i) idx.insert(idx.end(), {centre, centre + 1 + i, centre + 2 + i});
    return std::make_unique<Mesh>(v, idx);
}

std::unique_ptr<Mesh> torus(float major, float minor, int rings, int sides)
{
    std::vector<Vertex> v;
    std::vector<unsigned> idx;
    for (int i = 0; i <= rings; ++i) {
        float u = static_cast<float>(i) / rings;
        float a = u * 2.0f * PI;
        Vec3 ringDir(std::cos(a), std::sin(a), 0.0f);
        for (int j = 0; j <= sides; ++j) {
            float w = static_cast<float>(j) / sides;
            float b = w * 2.0f * PI;
            Vec3 n = ringDir * std::cos(b) + Vec3(0, 0, 1) * std::sin(b);
            v.push_back({ringDir * major + n * minor, n, {u, w}});
        }
    }
    for (int i = 0; i < rings; ++i) {
        for (int j = 0; j < sides; ++j) {
            unsigned a = i * (sides + 1) + j;
            unsigned b = a + sides + 1;
            idx.insert(idx.end(), {a, b, a + 1, a + 1, b, b + 1});
        }
    }
    return std::make_unique<Mesh>(v, idx);
}

} // namespace prim
