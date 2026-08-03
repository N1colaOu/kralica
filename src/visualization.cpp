// BodyVis.cpp
#include "../include/visualization.h"
#include <cmath>
#include <algorithm>
#include <iostream>

// ----------------------------------------------------------------------
// Internal shader sources (embedded as strings)
// ----------------------------------------------------------------------
static const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 uProj;
uniform vec2 uOffset;
uniform float uScale;

void main() {
    vec2 worldPos = aPos * uScale + uOffset;
    gl_Position = uProj * vec4(worldPos, 0.0, 1.0);
}
)";

static const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;

void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

// ----------------------------------------------------------------------
// Helper: compile a shader
// ----------------------------------------------------------------------
static GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
    }
    return shader;
}

// ----------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------
BodyVis::BodyVis(int winW, int winH, float worldSize, const std::string& title)
    : width(winW), height(winH), worldSize(worldSize)
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(winW, winH, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    // Load OpenGL functions with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, winW, winH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setupShaders();
    createCircleMesh(64);
}

BodyVis::~BodyVis() {
    glDeleteVertexArrays(1, &circleVAO);
    glDeleteBuffers(1, &circleVBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
}

// ----------------------------------------------------------------------
// Shader setup and orthographic projection
// ----------------------------------------------------------------------
void BodyVis::setupShaders() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader linking error:\n" << infoLog << std::endl;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Get uniform locations
    uProjLoc   = glGetUniformLocation(shaderProgram, "uProj");
    uOffsetLoc = glGetUniformLocation(shaderProgram, "uOffset");
    uScaleLoc  = glGetUniformLocation(shaderProgram, "uScale");
    uColorLoc  = glGetUniformLocation(shaderProgram, "uColor");

    // Create orthographic projection matrix (left, right, bottom, top)
    // Y is up, world centre at origin, visible worldSize units horizontally.
    float half = worldSize * 0.5f;
    float aspect = (float)width / (float)height;
    float orthoLeft   = -half * aspect;
    float orthoRight  =  half * aspect;
    float orthoBottom = -half;
    float orthoTop    =  half;

    float projMat[16] = {
        2.0f/(orthoRight-orthoLeft), 0, 0, 0,
        0, 2.0f/(orthoTop-orthoBottom), 0, 0,
        0, 0, -1, 0,
        -(orthoRight+orthoLeft)/(orthoRight-orthoLeft),
        -(orthoTop+orthoBottom)/(orthoTop-orthoBottom),
        0, 1
    };

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(uProjLoc, 1, GL_FALSE, projMat);
    glUseProgram(0);
}

// ----------------------------------------------------------------------
// Create a unit circle as a triangle fan (radius = 1, centre at origin)
// ----------------------------------------------------------------------
void BodyVis::createCircleMesh(int segments) {
    std::vector<float> vertices;
    // centre vertex
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.14159265f * i / segments;
        vertices.push_back(cosf(angle));
        vertices.push_back(sinf(angle));
    }
    circleVertexCount = segments + 2;   // centre + circumference points (first and last duplicate)

    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);
    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// ----------------------------------------------------------------------
// Public interface
// ----------------------------------------------------------------------
bool BodyVis::isOpen() const {
    return !glfwWindowShouldClose(window);
}

void BodyVis::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void BodyVis::drawBodies(const std::vector<Body>& bodies,
                         const std::vector<std::array<float,3>>& colors)
{
    // Use a constant radius – no mass scaling
    const float radius = 0.01f * worldSize;   // adjust multiplier as needed

    glUseProgram(shaderProgram);
    glBindVertexArray(circleVAO);

    for (size_t i = 0; i < bodies.size(); ++i) {
        const Vector3d& pos = bodies[i].get_pos();

        glUniform2f(uOffsetLoc, pos.get_x(), pos.get_y());
        glUniform1f(uScaleLoc, radius);

        if (i < colors.size())
            glUniform3fv(uColorLoc, 1, colors[i].data());
        else
            glUniform3f(uColorLoc, 1.0f, 1.0f, 1.0f);

        glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertexCount);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void BodyVis::drawBodies(const std::vector<Body>& bodies,
                         float r, float g, float b)
{
    const float radius = 0.005f * worldSize;

    glUseProgram(shaderProgram);
    glBindVertexArray(circleVAO);
    glUniform3f(uColorLoc, r, g, b);

    for (const auto& body : bodies) {
        glUniform2f(uOffsetLoc, body.get_pos().get_x(), body.get_pos().get_y());
        glUniform1f(uScaleLoc, radius);
        glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertexCount);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void BodyVis::display() {
    glfwSwapBuffers(window);
}

bool BodyVis::processEvents() {
    glfwPollEvents();
    return !glfwWindowShouldClose(window);
}