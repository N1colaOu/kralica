#include <renderer-ai.h>
#include <camera-ai.h>

#include <body.h>          // <-- your physics Body
#include <vector.h>        // <-- your Vector

#include <algorithm>
#include <cmath>
#include <limits>

namespace {
// Log-mass -> colour ramp. Cold blue for light bodies, warm orange for heavy ones.
glm::vec3 massToColor(float t) {
    const glm::vec3 cold(0.30f, 0.55f, 1.00f);
    const glm::vec3 mid (0.90f, 0.95f, 1.00f);
    const glm::vec3 hot (1.00f, 0.60f, 0.20f);
    return t < 0.5f ? glm::mix(cold, mid, t * 2.0f)
                    : glm::mix(mid, hot, (t - 0.5f) * 2.0f);
}
}

Renderer::~Renderer() { shutdown(); }

void Renderer::init(const std::string& shaderDirectory) {
    m_shader.loadFromFiles(shaderDirectory + "/body-ai.vert",
                           shaderDirectory + "/body-ai.frag");

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void*>(offsetof(Vertex, color)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void*>(offsetof(Vertex, size)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_DEPTH_TEST);
}

void Renderer::shutdown() {
    if (m_vbo) { glDeleteBuffers(1, &m_vbo);      m_vbo = 0; }
    if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
    m_capacity = m_vertexCount = 0;
    m_staging.clear();
}

void Renderer::ensureCapacity(std::size_t count) {
    if (count <= m_capacity) return;
    const std::size_t cap = std::max<std::size_t>(count, m_capacity + m_capacity/2 + 64);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(cap * sizeof(Vertex)),
                 nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_capacity = cap;
}

void Renderer::upload(const std::vector<kralica::Body>& bodies) {
    m_staging.clear();
    m_vertexCount = 0;
    if (bodies.empty()) return;

    // log-mass range for colour/size normalisation
    double minMass = std::numeric_limits<double>::max();
    double maxMass = 0.0;
    for (const auto& b : bodies) {
        const double m = b.get_mass();
        if (m <= 0.0) continue;
        minMass = std::min(minMass, m);
        maxMass = std::max(maxMass, m);
    }
    if (maxMass <= 0.0) return;
    const float logMin   = static_cast<float>(std::log(minMass));
    const float logRange = std::max(static_cast<float>(std::log(maxMass)) - logMin, 1e-6f);

    m_staging.reserve(bodies.size());
    for (const auto& b : bodies) {
        const double m = std::max(b.get_mass(), minMass);
        const float t  = std::clamp((static_cast<float>(std::log(m)) - logMin) / logRange,
                                    0.0f, 1.0f);

        const kralica::Vector2d& p = b.get_pos();

        Vertex v;
        v.position = glm::vec3(static_cast<float>(p[0]),
                               static_cast<float>(p[1]),
                               0.0f);
        v.color    = massToColor(t);
        // World-space radius: ~0.006 for the lightest body, ~0.05 for the heaviest.
        v.size     = glm::mix(0.006f, 0.05f, t * t);
        m_staging.push_back(v);
    }

    ensureCapacity(m_staging.size());

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    static_cast<GLsizeiptr>(m_staging.size() * sizeof(Vertex)),
                    m_staging.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_vertexCount = m_staging.size();
}

void Renderer::draw(const Camera& camera, float pointScale) {
    glClearColor(m_bg.r, m_bg.g, m_bg.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (m_vertexCount == 0) return;

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);   // additive
    glDisable(GL_DEPTH_TEST);

    // pixels-per-world-unit at distance 1 (from the perspective projection)
    const float pixelScale =
        static_cast<float>(camera.viewportHeight()) /
        (2.0f * std::tan(camera.fovY() * 0.5f));

    m_shader.bind();
    m_shader.setMat4 ("uView",       camera.viewMatrix());
    m_shader.setMat4 ("uProjection", camera.projectionMatrix());
    m_shader.setFloat("uPixelScale", pixelScale);
    m_shader.setFloat("uPointScale", pointScale);
    m_shader.setFloat("uGlow",       m_glow);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(m_vertexCount));
    glBindVertexArray(0);

    Shader::unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}