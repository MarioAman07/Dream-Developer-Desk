// Math.h - vectors and matrices written by hand, the same way as in common.h of
// our assignments (no math library). The names follow GLSL (dot, cross,
// normalize, mix), so the C++ code reads like the shaders.
//
// Matrices are stored column by column like in OpenGL: the element in row r and
// column c is m[c * 4 + r]. That is why they are sent to the shaders with
// glUniformMatrix4fv(..., GL_FALSE, m) without transposing.
#pragma once

#include <cmath>

const float PI = 3.14159265358979f;

inline float radians(float degrees) { return degrees * PI / 180.0f; }
inline float mix(float a, float b, float t) { return a + (b - a) * t; }

// Moves `current` smoothly towards `target`: every second the remaining
// distance shrinks by the factor exp(-rate). Because dt is inside the formula,
// the motion is the same at 30 FPS and at 300 FPS (frame-rate independent).
inline float ease(float current, float target, float rate, float dt)
{
    return current + (target - current) * (1.0f - std::exp(-rate * dt));
}

// ---------------------------------------------------------------- vectors

struct Vec2 {
    float x = 0.0f, y = 0.0f;
    Vec2() = default;
    explicit Vec2(float s) : x(s), y(s) {}
    Vec2(float x_, float y_) : x(x_), y(y_) {}
};

inline Vec2 operator*(const Vec2& a, float s) { return Vec2(a.x * s, a.y * s); }

struct Vec3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;
    Vec3() = default;
    explicit Vec3(float s) : x(s), y(s), z(s) {}
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vec3& operator+=(const Vec3& b) { x += b.x; y += b.y; z += b.z; return *this; }
};

inline Vec3 operator+(const Vec3& a, const Vec3& b) { return Vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline Vec3 operator-(const Vec3& a, const Vec3& b) { return Vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline Vec3 operator-(const Vec3& a) { return Vec3(-a.x, -a.y, -a.z); }
inline Vec3 operator*(const Vec3& a, float s) { return Vec3(a.x * s, a.y * s, a.z * s); }
inline Vec3 operator*(float s, const Vec3& a) { return a * s; }
inline Vec3 operator/(const Vec3& a, float s) { return Vec3(a.x / s, a.y / s, a.z / s); }
// Component-wise product, e.g. light colour * material colour (as in GLSL).
inline Vec3 operator*(const Vec3& a, const Vec3& b) { return Vec3(a.x * b.x, a.y * b.y, a.z * b.z); }

inline float dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

inline Vec3 cross(const Vec3& a, const Vec3& b)
{
    return Vec3(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
}

inline float length(const Vec3& a) { return std::sqrt(dot(a, a)); }

inline Vec3 normalize(const Vec3& a)
{
    float len = length(a);
    return len > 0.0f ? a / len : a;
}

// Linear interpolation a + (b - a) * t, like mix() in GLSL.
inline Vec3 mix(const Vec3& a, const Vec3& b, float t) { return a + (b - a) * t; }

inline Vec3 clamp(const Vec3& v, const Vec3& lo, const Vec3& hi)
{
    return Vec3(std::fmax(lo.x, std::fmin(v.x, hi.x)),
                std::fmax(lo.y, std::fmin(v.y, hi.y)),
                std::fmax(lo.z, std::fmin(v.z, hi.z)));
}

// ---------------------------------------------------------------- matrices

// 3 x 3 matrix, column by column (used only for the normal matrix).
struct Mat3 {
    float m[9] = {1, 0, 0,
                  0, 1, 0,
                  0, 0, 1};
};

// 4 x 4 matrix, column by column. A new matrix is the identity.
struct Mat4 {
    float m[16] = {1, 0, 0, 0,
                   0, 1, 0, 0,
                   0, 0, 1, 0,
                   0, 0, 0, 1};

    float& at(int row, int col) { return m[col * 4 + row]; }
    float at(int row, int col) const { return m[col * 4 + row]; }

    // Column 0, 1, 2 = the x, y, z axes of the frame, column 3 = its origin.
    Vec3 column(int c) const { return Vec3(m[c * 4], m[c * 4 + 1], m[c * 4 + 2]); }
};

inline Mat4 operator*(const Mat4& a, const Mat4& b)
{
    Mat4 r;
    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 4; ++col) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) sum += a.at(row, k) * b.at(k, col);
            r.at(row, col) = sum;
        }
    return r;
}

// ---- affine transformations (lecture "Transformations")

inline Mat4 translation(const Vec3& d)
{
    Mat4 r;
    r.at(0, 3) = d.x;
    r.at(1, 3) = d.y;
    r.at(2, 3) = d.z;
    return r;
}

inline Mat4 scaling(const Vec3& s)
{
    Mat4 r;
    r.at(0, 0) = s.x;
    r.at(1, 1) = s.y;
    r.at(2, 2) = s.z;
    return r;
}

inline Mat4 rotationX(float degreesAngle)
{
    float c = std::cos(radians(degreesAngle)), s = std::sin(radians(degreesAngle));
    Mat4 r;
    r.at(1, 1) = c;  r.at(1, 2) = -s;
    r.at(2, 1) = s;  r.at(2, 2) = c;
    return r;
}

inline Mat4 rotationY(float degreesAngle)
{
    float c = std::cos(radians(degreesAngle)), s = std::sin(radians(degreesAngle));
    Mat4 r;
    r.at(0, 0) = c;  r.at(0, 2) = s;
    r.at(2, 0) = -s; r.at(2, 2) = c;
    return r;
}

inline Mat4 rotationZ(float degreesAngle)
{
    float c = std::cos(radians(degreesAngle)), s = std::sin(radians(degreesAngle));
    Mat4 r;
    r.at(0, 0) = c;  r.at(0, 1) = -s;
    r.at(1, 0) = s;  r.at(1, 1) = c;
    return r;
}

// ---- viewing (lecture "Viewing")

// Camera at `eye` looking at `at`. Rows of the rotation part are the camera
// axes s (right), u (up) and -f (backwards); the last column moves eye to 0.
inline Mat4 lookAt(const Vec3& eye, const Vec3& at, const Vec3& up)
{
    Vec3 f = normalize(at - eye);
    Vec3 s = normalize(cross(f, up));
    Vec3 u = cross(s, f);

    Mat4 r;
    r.at(0, 0) = s.x;  r.at(0, 1) = s.y;  r.at(0, 2) = s.z;  r.at(0, 3) = -dot(s, eye);
    r.at(1, 0) = u.x;  r.at(1, 1) = u.y;  r.at(1, 2) = u.z;  r.at(1, 3) = -dot(u, eye);
    r.at(2, 0) = -f.x; r.at(2, 1) = -f.y; r.at(2, 2) = -f.z; r.at(2, 3) = dot(f, eye);
    return r;
}

// Perspective projection with a vertical field of view in degrees.
inline Mat4 perspective(float fovyDegrees, float aspect, float zNear, float zFar)
{
    float t = 1.0f / std::tan(radians(fovyDegrees) * 0.5f);
    Mat4 r;
    r.at(0, 0) = t / aspect;
    r.at(1, 1) = t;
    r.at(2, 2) = -(zFar + zNear) / (zFar - zNear);
    r.at(2, 3) = -2.0f * zFar * zNear / (zFar - zNear);
    r.at(3, 2) = -1.0f;
    r.at(3, 3) = 0.0f;
    return r;
}

// ---- normal matrix (lecture "Shading": normals under non-uniform scaling)

// Transpose of the inverse of the upper-left 3 x 3 part of the model matrix.
// The inverse is the adjugate (matrix of cofactors, transposed) divided by the
// determinant, so the transposed inverse is simply cofactors / determinant.
inline Mat3 normalMatrix(const Mat4& a)
{
    float a00 = a.at(0, 0), a01 = a.at(0, 1), a02 = a.at(0, 2);
    float a10 = a.at(1, 0), a11 = a.at(1, 1), a12 = a.at(1, 2);
    float a20 = a.at(2, 0), a21 = a.at(2, 1), a22 = a.at(2, 2);

    // cofactor[row][col]
    float c00 = a11 * a22 - a12 * a21, c01 = a12 * a20 - a10 * a22, c02 = a10 * a21 - a11 * a20;
    float c10 = a02 * a21 - a01 * a22, c11 = a00 * a22 - a02 * a20, c12 = a01 * a20 - a00 * a21;
    float c20 = a01 * a12 - a02 * a11, c21 = a02 * a10 - a00 * a12, c22 = a00 * a11 - a01 * a10;

    float det = a00 * c00 + a01 * c01 + a02 * c02;
    float k = (std::fabs(det) > 1e-12f) ? 1.0f / det : 0.0f;

    Mat3 r;   // column by column: r.m[col * 3 + row] = cofactor[row][col] / det
    r.m[0] = c00 * k; r.m[1] = c10 * k; r.m[2] = c20 * k;
    r.m[3] = c01 * k; r.m[4] = c11 * k; r.m[5] = c21 * k;
    r.m[6] = c02 * k; r.m[7] = c12 * k; r.m[8] = c22 * k;
    return r;
}
