#include <window-ai.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace {
void glfwErrorCallback(int code, const char* description) {
    std::cerr << "[GLFW] (" << code << ") " << description << std::endl;
}
}

Window::Window(int width, int height, const std::string& title, bool vsync)
    : m_width(width), m_height(height)
{
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) throw std::runtime_error("Window: failed to initialise GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) { glfwTerminate(); throw std::runtime_error("Window: create failed"); }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(vsync ? 1 : 0);

#if NBODY_GLAD_VERSION == 1
    const int loaded = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
#else
    const int loaded = gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress));
#endif
    if (!loaded) {
        glfwDestroyWindow(m_window); glfwTerminate();
        throw std::runtime_error("Window: GLAD load failed");
    }

    glfwGetFramebufferSize(m_window, &m_width, &m_height);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, onFramebufferSize);
    glfwSetCursorPosCallback(m_window, onCursorPos);
    glfwSetMouseButtonCallback(m_window, onMouseButton);
    glfwSetScrollCallback(m_window, onScroll);
    glfwSetKeyCallback(m_window, onKey);

    std::cout << "OpenGL " << glGetString(GL_VERSION)
              << " | "   << glGetString(GL_RENDERER) << std::endl;
}

Window::~Window() {
    if (m_window) glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::newFrame() {
    m_input.mouseDeltaX = m_input.mouseDeltaY = m_input.scrollDeltaY = 0.0;
    std::fill(std::begin(m_input.keyPressed), std::end(m_input.keyPressed), false);
    glfwPollEvents();
}
void Window::swapBuffers()           { glfwSwapBuffers(m_window); }
bool Window::shouldClose() const     { return glfwWindowShouldClose(m_window) != 0; }
void Window::close()                 { glfwSetWindowShouldClose(m_window, GLFW_TRUE); }
void Window::setTitle(const std::string& title) { glfwSetWindowTitle(m_window, title.c_str()); }

bool Window::isKeyDown(int key) const {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return m_input.keyDown[key];
}
bool Window::wasKeyPressed(int key) const {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return m_input.keyPressed[key];
}
bool Window::isMouseDown(int button) const {
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:   return m_input.leftMouseDown;
        case GLFW_MOUSE_BUTTON_RIGHT:  return m_input.rightMouseDown;
        case GLFW_MOUSE_BUTTON_MIDDLE: return m_input.middleMouseDown;
        default: return false;
    }
}

void Window::onFramebufferSize(GLFWwindow* w, int width, int height) {
    if (Window* self = from(w)) { self->m_width = width; self->m_height = height; }
}
void Window::onCursorPos(GLFWwindow* w, double x, double y) {
    Window* self = from(w); if (!self) return;
    if (self->m_firstCursorEvent) {
        self->m_input.mouseX = x; self->m_input.mouseY = y;
        self->m_firstCursorEvent = false;
    }
    self->m_input.mouseDeltaX += x - self->m_input.mouseX;
    self->m_input.mouseDeltaY += y - self->m_input.mouseY;
    self->m_input.mouseX = x; self->m_input.mouseY = y;
}
void Window::onMouseButton(GLFWwindow* w, int button, int action, int) {
    Window* self = from(w); if (!self) return;
    const bool down = (action == GLFW_PRESS);
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:   self->m_input.leftMouseDown   = down; break;
        case GLFW_MOUSE_BUTTON_RIGHT:  self->m_input.rightMouseDown  = down; break;
        case GLFW_MOUSE_BUTTON_MIDDLE: self->m_input.middleMouseDown = down; break;
        default: break;
    }
}
void Window::onScroll(GLFWwindow* w, double, double yoffset) {
    if (Window* self = from(w)) self->m_input.scrollDeltaY += yoffset;
}
void Window::onKey(GLFWwindow* w, int key, int, int action, int) {
    Window* self = from(w);
    if (!self || key < 0 || key > GLFW_KEY_LAST) return;
    if (action == GLFW_PRESS)   { self->m_input.keyDown[key] = true;  self->m_input.keyPressed[key] = true; }
    if (action == GLFW_RELEASE) { self->m_input.keyDown[key] = false; }
}