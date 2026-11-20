// Rgb.h - CPU copy of rgbEffect() from shaders/scene.frag, so the RGB point
// lights (glow on the wall) have exactly the colour of the LEDs.
#pragma once

#include "core/Math.h"

#include <cmath>

namespace rgb {

inline float fract(float x) { return x - std::floor(x); }

// HSV -> RGB (lecture "Introduction", colour models). h, s, v are 0..1; the
// hue circle is cut into 6 sectors of 60 degrees.
inline Vec3 hsv2rgb(float h, float s, float v)
{
    float sector = fract(h) * 6.0f;
    int i = static_cast<int>(sector);
    float f = sector - static_cast<float>(i);   // position inside the sector
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));
    switch (i) {
    case 0: return Vec3(v, t, p);
    case 1: return Vec3(q, v, p);
    case 2: return Vec3(p, v, t);
    case 3: return Vec3(p, q, v);
    case 4: return Vec3(t, p, v);
    default: return Vec3(v, p, q);
    }
}

inline Vec3 effect(int mode, float phase, float time, float speed, const Vec3& staticColor)
{
    float t = time * speed;
    switch (mode) {
    case 0: return hsv2rgb(t * 0.12f + phase, 1.0f, 1.0f);   // rainbow wave
    case 1: return hsv2rgb(t * 0.08f, 1.0f, 1.0f);           // whole spectrum
    case 2: {                                                // breathing
        float b = 0.12f + 0.88f * (0.5f + 0.5f * std::sin(t * 2.0f));
        return hsv2rgb(t * 0.02f, 1.0f, b);
    }
    case 3: return staticColor;                              // static colour
    default: return Vec3(0.0f);                              // off
    }
}

} // namespace rgb
