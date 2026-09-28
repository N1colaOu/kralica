#pragma once
#include "glad-compact-ai.h"
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    Shader() = default;
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void loadFromFiles(const std::string& vs, const std::string& fs);
    void loadFromSource(const std::string& vs, const std::string& fs);
    void bind() const;
    static void unbind();
    GLuint id() const { return m_program; }

    void setFloat(const std::string&, float) const;
    void setInt  (const std::string&, int)   const;
    void setVec3 (const std::string&, const glm::vec3&) const;
    void setMat4 (const std::string&, const glm::mat4&) const;

private:
    GLint uniformLocation(const std::string&) const;
    void destroy();
    GLuint m_program = 0;
};