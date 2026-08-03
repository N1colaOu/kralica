// include/visualization.h
#pragma once
#include <../glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <array>                 // <--- added
#include "body.h"                // your Body / Vector3d

class BodyVis {
public:
    BodyVis(int winW, int winH, float worldSize,
            const std::string& title = "Gravity Simulation");
    ~BodyVis();

    bool isOpen() const;
    void clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);

    // Now use std::array<float,3> instead of float[3]
    void drawBodies(const std::vector<Body>& bodies,
                    const std::vector<std::array<float,3>>& colors);
    void drawBodies(const std::vector<Body>& bodies,
                    float r, float g, float b);

    void display();
    bool processEvents();

    GLFWwindow* getWindow() { return window; }

private:
    GLFWwindow* window;
    int width, height;
    float worldSize;

    GLuint shaderProgram;
    GLint uProjLoc, uOffsetLoc, uScaleLoc, uColorLoc;

    GLuint circleVAO, circleVBO;
    int circleVertexCount;

    void setupShaders();
    void createCircleMesh(int segments = 64);
};