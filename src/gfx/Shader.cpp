#include "gfx/Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

Shader::~Shader()
{
    if (m_program) glDeleteProgram(m_program);
}

std::string Shader::readFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file) {
        std::cerr << "[Shader] Cannot open " << path << "\n";
        return {};
    }
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

GLuint Shader::compile(GLenum type, const std::string& source, const std::string& path)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "[Shader] Compile error in " << path << ":\n" << log << "\n";
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Shader::load(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vsSource = readFile(vertexPath);
    std::string fsSource = readFile(fragmentPath);
    if (vsSource.empty() || fsSource.empty()) return false;

    GLuint vs = compile(GL_VERTEX_SHADER, vsSource, vertexPath);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fsSource, fragmentPath);
    if (!vs || !fs) {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return false;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "[Shader] Link error (" << vertexPath << " + " << fragmentPath << "):\n" << log << "\n";
        glDeleteProgram(program);
        return false;
    }

    if (m_program) glDeleteProgram(m_program);
    m_program = program;
    m_locations.clear();
    return true;
}

GLint Shader::location(const std::string& name) const
{
    auto it = m_locations.find(name);
    if (it != m_locations.end()) return it->second;
    GLint loc = glGetUniformLocation(m_program, name.c_str());
    m_locations.emplace(name, loc);
    return loc;
}

void Shader::setInt(const std::string& name, int value) const { glUniform1i(location(name), value); }
void Shader::setFloat(const std::string& name, float value) const { glUniform1f(location(name), value); }
void Shader::setVec2(const std::string& name, const Vec2& v) const { glUniform2f(location(name), v.x, v.y); }
void Shader::setVec3(const std::string& name, const Vec3& v) const { glUniform3f(location(name), v.x, v.y, v.z); }
void Shader::setMat3(const std::string& name, const Mat3& m) const { glUniformMatrix3fv(location(name), 1, GL_FALSE, m.m); }
void Shader::setMat4(const std::string& name, const Mat4& m) const { glUniformMatrix4fv(location(name), 1, GL_FALSE, m.m); }
