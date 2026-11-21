// App.h - window, main loop, input handling. Owns the scene, renderer, camera and UI.
#pragma once

#include "core/FpsCounter.h"
#include "gfx/Framebuffer.h"
#include "render/Renderer.h"
#include "scene/Camera.h"
#include "scene/DeskScene.h"
#include "ui/Gui.h"

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
    void applyCameraView(int index);

    // GLFW callbacks (forwarded from static functions)
    void onKey(int key, int action, int mods);
    void onMouseButton(int button, int action);
    void onCursor(double x, double y);
    void onScroll(double dy);
    void onResize(int width, int height);
    static App* self(GLFWwindow* w);

    GLFWwindow* m_window = nullptr;
    int m_fbWidth = 0;
    int m_fbHeight = 0;

    DeskScene m_scene;
    Renderer m_renderer;
    Camera m_camera;
    Gui m_gui;
    FpsCounter m_fps;

    // Off-screen framebuffer for the security camera picture (render-to-texture).
    Framebuffer m_feed;

    bool m_leftDown = false;
    bool m_rightDown = false;
    double m_lastX = 0.0;
    double m_lastY = 0.0;
};
