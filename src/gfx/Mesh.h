// Mesh.h - GPU geometry: one VAO with a VBO (interleaved vertices) and an EBO
// (indices). The data is uploaded once when the mesh is created (GL_STATIC_DRAW).
// Class layout follows LearnOpenGL "Mesh" (J. de Vries, learnopengl.com,
// CC BY-NC 4.0), see CREDITS.md.
#pragma once

#include "core/Math.h"

#include <glad/glad.h>

#include <vector>

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices);
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw() const;

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    GLsizei m_indexCount = 0;
};
