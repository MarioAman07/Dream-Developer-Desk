#include "scene/Camera.h"

#include <algorithm>
#include <cmath>

namespace {
const Vec3 kDefaultTarget(0.05f, 0.95f, -1.85f);
constexpr float kDefaultDistance = 2.7f;
constexpr float kDefaultYaw = 34.0f;     // degrees around Y, 0 = looking along -Z
constexpr float kDefaultPitch = 20.0f;   // degrees above the horizon
constexpr float kMinDistance = 0.6f;
constexpr float kMaxDistance = 7.5f;

// Unit vector from the target to the camera (spherical coordinates).
Vec3 orbitOffset(float yawDeg, float pitchDeg)
{
    float y = radians(yawDeg), p = radians(pitchDeg);
    return Vec3(std::sin(y) * std::cos(p), std::sin(p), std::cos(y) * std::cos(p));
}
} // namespace

Camera::Camera()
{
    reset();
    yaw = m_goalYaw;
    pitch = m_goalPitch;
    distance = m_goalDistance;
    target = m_goalTarget;
}

void Camera::reset()
{
    m_goalTarget = kDefaultTarget;
    m_goalDistance = kDefaultDistance;
    m_goalYaw = kDefaultYaw;
    m_goalPitch = kDefaultPitch;
}

void Camera::update(float dt)
{
    // Smooth, frame-rate independent motion towards the goal values (see ease in Math.h).
    yaw = ease(yaw, m_goalYaw, smoothing, dt);
    pitch = ease(pitch, m_goalPitch, smoothing, dt);
    distance = ease(distance, m_goalDistance, smoothing, dt);
    target = Vec3(ease(target.x, m_goalTarget.x, smoothing, dt),
                  ease(target.y, m_goalTarget.y, smoothing, dt),
                  ease(target.z, m_goalTarget.z, smoothing, dt));
}

void Camera::orbit(float yawDeg, float pitchDeg)
{
    m_goalYaw += yawDeg;
    m_goalPitch = std::clamp(m_goalPitch + pitchDeg, -10.0f, 85.0f);
}

void Camera::zoom(float amount)
{
    m_goalDistance = std::clamp(m_goalDistance * std::pow(0.9f, amount), kMinDistance, kMaxDistance);
}

void Camera::pan(float right, float up, float fwd)
{
    // Right and forward directions of the camera in the floor plane.
    float y = radians(yaw);
    Vec3 r(std::cos(y), 0.0f, -std::sin(y));
    Vec3 f(-std::sin(y), 0.0f, -std::cos(y));
    m_goalTarget += r * right + Vec3(0, 1, 0) * up + f * fwd;
    // Keep the target inside the room.
    m_goalTarget = clamp(m_goalTarget, Vec3(-2.8f, 0.1f, -2.4f), Vec3(2.8f, 2.8f, 2.4f));
}

void Camera::focus(const Vec3& t, float dist, float yawDeg, float pitchDeg)
{
    m_goalTarget = t;
    m_goalDistance = dist;
    // Take the short way round so the camera does not spin a full circle.
    float delta = std::remainder(yawDeg - m_goalYaw, 360.0f);
    m_goalYaw += delta;
    m_goalPitch = pitchDeg;
}

Vec3 Camera::position() const
{
    return target + orbitOffset(yaw, pitch) * distance;
}

Mat4 Camera::view() const
{
    return lookAt(position(), target, Vec3(0, 1, 0));
}

Mat4 Camera::projection(float aspect) const
{
    return perspective(fovDeg, aspect, nearPlane, farPlane);
}
