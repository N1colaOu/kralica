#pragma once
#include "glad-compact-ai.h"
#include "shader-ai.h"
#include <glm/glm.hpp>
#include <cstddef>
#include <string>
#include <vector>

class Camera;
namespace kralica { class Body; }

/// Draws a vector<kralica::Body> as additive glow points on the XY plane.
class Renderer {
public:
    Renderer() = default;
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void init(const std::string& shaderDirectory);
    void shutdown();

    void upload(const std::vector<kralica::Body>& bodies);
    void draw(const Camera& camera, float pointScale = 1.0f);

    void setBackgroundColor(const glm::vec3& c) { m_bg = c; }
    void setGlowStrength(float g)               { m_glow = g; }

private:
    struct Vertex { glm::vec3 position; glm::vec3 color; float size; };

    void ensureCapacity(std::size_t count);

    Shader m_shader;
    GLuint m_vao = 0, m_vbo = 0;
    std::size_t m_capacity = 0, m_vertexCount = 0;
    std::vector<Vertex> m_staging;

    glm::vec3 m_bg{0.008f, 0.010f, 0.020f};
    float m_glow = 1.0f;
};