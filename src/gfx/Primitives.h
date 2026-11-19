// Primitives.h - procedural geometry for every object in the scene.
// All shapes are centred at the origin; unit shapes have size 1 and are scaled
// by the scene node's meshScale.
#pragma once

#include "gfx/Mesh.h"

#include <memory>

namespace prim {

// Unit cube (-0.5..0.5), UVs 0..1 on every face.
std::unique_ptr<Mesh> cube();

// Box with real size in metres; UVs are in metres * uvPerMetre, so a texture
// keeps the same scale on every face (no stretching on long or thin boxes).
std::unique_ptr<Mesh> box(const Vec3& size, float uvPerMetre);

// Unit quad in the XY plane facing +Z, UVs 0..1 (screens, posters, sky).
std::unique_ptr<Mesh> quad();

// Cylinder along Y: radius 0.5, height 1, with caps.
std::unique_ptr<Mesh> cylinder(int segments = 32);

// UV sphere, radius 0.5.
std::unique_ptr<Mesh> sphere(int stacks = 20, int slices = 32);

// Cone along Y: base radius 0.5 at y = -0.5, tip at y = +0.5, with base cap.
std::unique_ptr<Mesh> cone(int segments = 32);

// Torus around the Z axis (lies in the XY plane), outer radius = major + minor.
std::unique_ptr<Mesh> torus(float major, float minor, int rings = 40, int sides = 12);

} // namespace prim
