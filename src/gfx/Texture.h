// Texture.h - 2D texture loaded from a PNG file with stb_image, with mipmaps
// (lecture "Texture Mapping": glGenerateMipmap + GL_LINEAR_MIPMAP_LINEAR).
// Loading with stb_image follows LearnOpenGL "Textures" (J. de Vries,
// learnopengl.com, CC BY-NC 4.0), see CREDITS.md.
#pragma once

#include <glad/glad.h>

#include <string>

class Texture {
public:
    Texture() = default;
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    bool loadFromFile(const std::string& path);
    // 1x1 texture used when an image is missing, so the program still runs.
    void createSolid(unsigned char r, unsigned char g, unsigned char b);

    GLuint id() const { return m_id; }

private:
    GLuint m_id = 0;
};
