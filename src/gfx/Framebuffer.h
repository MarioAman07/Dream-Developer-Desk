// Framebuffer.h - off-screen render target: a frame buffer object with a colour
// texture and a depth renderbuffer (lecture "Pixels and Buffers": render to
// texture; Assignment 4 used FBOs with renderbuffers for picking).
// Used for the security camera picture shown on a monitor.
// Set-up order follows LearnOpenGL "Framebuffers" (J. de Vries, learnopengl.com,
// CC BY-NC 4.0), see CREDITS.md.
#pragma once

#include <glad/glad.h>

class Framebuffer {
public:
    Framebuffer() = default;
    ~Framebuffer();
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    bool create(int width, int height);
    void bind() const;
    static void bindDefault(int windowWidth, int windowHeight);

    GLuint colorTexture() const { return m_color; }

private:
    void release();

    GLuint m_fbo = 0;
    GLuint m_color = 0;
    GLuint m_depth = 0;
    int m_width = 0;
    int m_height = 0;
};
