// FpsCounter.h - averages the frame times over a quarter of a second for the
// FPS shown in the window title and in the panel (R7).
#pragma once

class FpsCounter {
public:
    // Returns true when new averaged values are ready (about 4 times per second).
    bool frame(float dt)
    {
        m_accum += dt;
        ++m_frames;
        if (m_accum >= 0.25f) {
            fps = static_cast<float>(m_frames) / m_accum;
            ms = 1000.0f * m_accum / static_cast<float>(m_frames);
            m_accum = 0.0f;
            m_frames = 0;
            return true;
        }
        return false;
    }

    float fps = 0.0f;
    float ms = 0.0f;

private:
    float m_accum = 0.0f;
    int m_frames = 0;
};
