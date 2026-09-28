#pragma once
#include "glad-compact-ai.h"
#include <GLFW/glfw3.h>
#include <string>

struct InputState {
    double mouseX = 0.0, mouseY = 0.0;
    double mouseDeltaX = 0.0, mouseDeltaY = 0.0;
    double scrollDeltaY = 0.0;
    bool leftMouseDown = false, rightMouseDown = false, middleMouseDown = false;
    bool keyDown[GLFW_KEY_LAST + 1] = {};
    bool keyPressed[GLFW_KEY_LAST + 1] = {};
};

class Window {
public:
    Window(int width, int height, const std::string& title, bool vsync = true);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void newFrame();
    void swapBuffers();
    bool shouldClose() const;
    void close();
    void setTitle(const std::string& title);

    GLFWwindow* handle() const { return m_window; }
    int   width()  const { return m_width; }
    int   height() const { return m_height; }
    float aspect() const { return m_height > 0 ? float(m_width)/float(m_height) : 1.0f; }

    const InputState& input() const { return m_input; }
    bool isKeyDown(int key) const;
    bool wasKeyPressed(int key) const;
    bool isMouseDown(int button) const;

    double time() const { return glfwGetTime(); }

private:
    static void onFramebufferSize(GLFWwindow*, int, int);
    static void onCursorPos(GLFWwindow*, double, double);
    static void onMouseButton(GLFWwindow*, int, int, int);
    static void onScroll(GLFWwindow*, double, double);
    static void onKey(GLFWwindow*, int, int, int, int);
    static Window* from(GLFWwindow* w) { return static_cast<Window*>(glfwGetWindowUserPointer(w)); }

    GLFWwindow* m_window = nullptr;
    int m_width = 0, m_height = 0;
    bool m_firstCursorEvent = true;
    InputState m_input;
};