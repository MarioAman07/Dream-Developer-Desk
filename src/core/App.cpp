#include "core/App.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdio>
#include <iostream>

namespace {
constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;

void glfwErrorCallback(int code, const char* description)
{
    std::cerr << "[GLFW] Error " << code << ": " << description << "\n";
}
} // namespace

App::~App() { shutdown(); }

App* App::self(GLFWwindow* w) { return static_cast<App*>(glfwGetWindowUserPointer(w)); }

bool App::init()
{
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 4);   // 4 samples per pixel: anti-aliasing (as in Assignment 4)

    m_window = glfwCreateWindow(kWindowWidth, kWindowHeight, "Dream Developer Desk", nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Could not create an OpenGL 3.3 core window.\n";
        return false;
    }
    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Could not load OpenGL functions (GLAD).\n";
        return false;
    }
    glfwSwapInterval(1);   // VSync
    std::cout << "[GL] " << glGetString(GL_VERSION) << " | " << glGetString(GL_RENDERER) << "\n";

    // GLFW calls these functions; they forward the events to this App object.
    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, [](GLFWwindow* w, int k, int, int a, int m) { self(w)->onKey(k, a, m); });
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* w, int x, int y) { self(w)->onResize(x, y); });
    glfwGetFramebufferSize(m_window, &m_fbWidth, &m_fbHeight);

    glEnable(GL_MULTISAMPLE);

    if (!m_renderer.init()) {
        std::cerr << "Shader loading failed - see messages above.\n";
        return false;
    }
    m_scene.init();
    return true;
}

void App::shutdown()
{
    if (!m_window) return;
    glfwDestroyWindow(m_window);
    glfwTerminate();
    m_window = nullptr;
}

// ---------------------------------------------------------------- input

void App::onKey(int key, int action, int mods)
{
    if (action != GLFW_PRESS) return;
    SceneSettings& s = m_scene.settings;

    switch (key) {
    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(m_window, GLFW_TRUE); break;
    case GLFW_KEY_1: case GLFW_KEY_2: case GLFW_KEY_3: case GLFW_KEY_4:
        m_scene.applyArrangementPreset(static_cast<Arrangement>(key - GLFW_KEY_1));
        break;
    case GLFW_KEY_M: m_scene.nextArrangement(); break;
    case GLFW_KEY_L: s.deskLamp = !s.deskLamp; break;
    case GLFW_KEY_F: s.ceilingLight = !s.ceilingLight; break;
    case GLFW_KEY_R: s.rgbMode = (s.rgbMode + ((mods & GLFW_MOD_SHIFT) ? kRgbModeCount - 1 : 1)) % kRgbModeCount; break;
    case GLFW_KEY_K: m_scene.spinChair(420.0f); break;
    default: break;
    }
}

void App::onResize(int width, int height)
{
    m_fbWidth = width;
    m_fbHeight = height;
}

// ---------------------------------------------------------------- frame

void App::renderFrame()
{
    // Draw the scene into the window.
    Framebuffer::bindDefault(m_fbWidth, m_fbHeight);
    glClearColor(0.07f, 0.07f, 0.085f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderView main;
    // Fixed camera: the default overview of the desk.
    main.eye = Vec3(1.47f, 1.87f, 0.25f);
    main.view = lookAt(main.eye, Vec3(0.05f, 0.95f, -1.85f), Vec3(0.0f, 1.0f, 0.0f));
    main.projection = perspective(55.0f, static_cast<float>(m_fbWidth) / std::max(1, m_fbHeight), 0.03f, 60.0f);
    m_renderer.render(m_scene, main);
}

int App::run()
{
    if (!init()) return 1;

    double last = glfwGetTime();
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        double now = glfwGetTime();
        float dt = std::min(static_cast<float>(now - last), 0.1f);
        last = now;

        // FPS counter in the window title (R7).
        if (m_fps.frame(dt)) {
            char title[128];
            std::snprintf(title, sizeof(title), "Dream Developer Desk  |  %.0f FPS  (%.2f ms)", m_fps.fps, m_fps.ms);
            glfwSetWindowTitle(m_window, title);
        }

        m_scene.update(dt, static_cast<float>(now));

        if (m_fbWidth > 0 && m_fbHeight > 0) {
            renderFrame();
        }
        glfwSwapBuffers(m_window);
    }
    shutdown();
    return 0;
}
