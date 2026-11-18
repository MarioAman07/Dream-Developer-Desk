// App.h - window and main loop.
#pragma once

#include "core/FpsCounter.h"

struct GLFWwindow;

class App {
public:
    App() = default;
    ~App();
    int run();

private:
    bool init();
    void shutdown();
    void renderFrame();

    // GLFW callbacks (forwarded from static functions)
    void onKey(int key, int action, int mods);
    void onResize(int width, int height);
    static App* self(GLFWwindow* w);

    GLFWwindow* m_window = nullptr;
    int m_fbWidth = 0;
    int m_fbHeight = 0;

    FpsCounter m_fps;
};
