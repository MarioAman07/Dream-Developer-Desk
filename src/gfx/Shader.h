// Shader.h - loads, compiles and links a GLSL vertex + fragment shader pair
// (the same steps as compileShader / createShaderProgram in common.h of our
// assignments) and sets uniforms by name.
// Class layout follows LearnOpenGL "Shaders" (J. de Vries, learnopengl.com,
// CC BY-NC 4.0), see CREDITS.md.
#pragma once

#include "core/Math.h"

#include <glad/glad.h>

#include <string>
#include <unordered_map>

class Shader {
public:
    Shader() = default;
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Returns false and prints the compiler log if anything fails.
    bool load(const std::string& vertexPath, const std::string& fragmentPath);

    void use() const { glUseProgram(m_program); }

    void setInt(const std::string& name, int value) const;
    void setBool(const std::string& name, bool value) const { setInt(name, value ? 1 : 0); }
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const Vec2& v) const;
    void setVec3(const std::string& name, const Vec3& v) const;
    void setMat3(const std::string& name, const Mat3& m) const;
    void setMat4(const std::string& name, const Mat4& m) const;

private:
    GLint location(const std::string& name) const;
    static std::string readFile(const std::string& path);
    static GLuint compile(GLenum type, const std::string& source, const std::string& path);

    GLuint m_program = 0;
    // The location of every uniform is asked from OpenGL only once and then remembered.
    mutable std::unordered_map<std::string, GLint> m_locations;
};
