// Camera.h - orbit camera around the desk. The eye moves on a sphere around a
// target point (yaw, pitch, distance) and always looks at the target with
// lookAt - the same idea as the camera on a cylinder in our Assignments 3 and 4.
// Input changes the *goal* values; update(dt) moves the real values towards
// them with exponential smoothing, so motion is smooth and frame-rate independent.
#pragma once

#include "core/Math.h"

class Camera {
public:
    Camera();

    void update(float dt);
    void reset();

    void orbit(float yawDeg, float pitchDeg);      // mouse drag / arrow keys
    void zoom(float amount);                        // mouse wheel
    void pan(float right, float up, float forward); // WASD / QE move the target

    // Smoothly fly to a new view (camera presets).
    void focus(const Vec3& target, float distance, float yawDeg, float pitchDeg);

    Vec3 position() const;
    Mat4 view() const;
    Mat4 projection(float aspect) const;

    float fovDeg = 55.0f;
    float nearPlane = 0.03f;
    float farPlane = 60.0f;
    float smoothing = 12.0f;   // higher = snappier

    // Current values (public so the UI can show them).
    float yaw = 0.0f, pitch = 0.0f, distance = 0.0f;
    Vec3 target;

private:
    float m_goalYaw = 0.0f, m_goalPitch = 0.0f, m_goalDistance = 0.0f;
    Vec3 m_goalTarget;
};
