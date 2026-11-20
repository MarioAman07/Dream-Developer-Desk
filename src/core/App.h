// App.h - window, main loop, input handling. Owns the scene, renderer, camera and UI.
#pragma once

#include "core/FpsCounter.h"
#include "gfx/Framebuffer.h"
#include "render/Renderer.h"
#include "scene/DeskScene.h"

struct GLFWwindow;

class App {
public:
    App() = default;
    ~App();
    int run();

private:
    bool init();
    void shutdown();
    void processInput(float dt);
    void renderFrame();

    // GLFW callbacks (forwarded from static functions)
    void onKey(int key, int action, int mods);
    void onResize(int width, int height);
    static App* self(GLFWwindow* w);

    GLFWwindow* m_window = nullptr;
    int m_fbWidth = 0;
    int m_fbHeight = 0;

    DeskScene m_scene;
    Renderer m_renderer;
    FpsCounter m_fps;
};
