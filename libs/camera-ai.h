#pragma once
#include <glm/glm.hpp>

/// Orbit camera. Defaults tuned for a 2D XY-plane view (pitch=yaw=0 looks down -Z).
class Camera {
public:
    void setViewport(int w, int h);
    void orbit(float dx, float dy);
    void pan  (float dx, float dy);
    void zoom (float steps);

    void setTarget(const glm::vec3& t) { m_target = t; }
    void setDistance(float d);
    void setAngles(float yaw, float pitch);

    const glm::vec3& target() const { return m_target; }
    float distance() const { return m_distance; }
    float fovY()     const { return m_fovY; }
    int   viewportWidth()  const { return m_vw; }
    int   viewportHeight() const { return m_vh; }

    glm::vec3 position() const;
    glm::vec3 forward()  const;
    glm::vec3 right()    const;
    glm::vec3 up()       const;

    glm::mat4 viewMatrix()       const;
    glm::mat4 projectionMatrix() const;

private:
    glm::vec3 m_target{0.0f};
    float m_distance = 3.0f;
    float m_yaw   = 0.0f;   // radians
    float m_pitch = 0.0f;   // radians
    float m_fovY  = glm::radians(50.0f);
    int   m_vw = 1280, m_vh = 960;

    float m_orbitSpeed = 0.006f;
    float m_panSpeed   = 0.0018f;
    float m_zoomSpeed  = 0.12f;
};