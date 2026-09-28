#include <camera-ai.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

namespace {
constexpr float kMinDist  = 1e-3f;
constexpr float kMaxDist  = 1e6f;
constexpr float kMaxPitch = 1.5533f;
}

void Camera::setViewport(int w, int h) { m_vw = std::max(w,1); m_vh = std::max(h,1); }
void Camera::setDistance(float d)      { m_distance = std::clamp(d, kMinDist, kMaxDist); }
void Camera::setAngles(float y, float p) {
    m_yaw = y;
    m_pitch = std::clamp(p, -kMaxPitch, kMaxPitch);
}
void Camera::orbit(float dx, float dy) {
    m_yaw   -= dx * m_orbitSpeed;
    m_pitch += dy * m_orbitSpeed;
    m_pitch = std::clamp(m_pitch, -kMaxPitch, kMaxPitch);
}
void Camera::pan(float dx, float dy) {
    const float s = m_distance * m_panSpeed;
    m_target += (-dx * right() + dy * up()) * s;
}
void Camera::zoom(float steps) { setDistance(m_distance * std::exp(-steps * m_zoomSpeed)); }

glm::vec3 Camera::position() const {
    const float cp = std::cos(m_pitch);
    return m_target + m_distance * glm::vec3(cp * std::sin(m_yaw),
                                             std::sin(m_pitch),
                                             cp * std::cos(m_yaw));
}
glm::vec3 Camera::forward() const { return glm::normalize(m_target - position()); }
glm::vec3 Camera::right()   const { return glm::normalize(glm::cross(forward(), glm::vec3(0,1,0))); }
glm::vec3 Camera::up()      const { return glm::cross(right(), forward()); }

glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(position(), m_target, glm::vec3(0,1,0));
}
glm::mat4 Camera::projectionMatrix() const {
    const float aspect = float(m_vw) / float(m_vh);
    const float nearP  = std::max(1e-4f, m_distance * 0.005f);
    const float farP   = std::max(100.0f, m_distance * 200.0f);
    return glm::perspective(m_fovY, aspect, nearP, farP);
}