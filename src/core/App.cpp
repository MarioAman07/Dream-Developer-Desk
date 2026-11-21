#include "core/App.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdio>
#include <iostream>

namespace {
constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;
constexpr int kFeedWidth = 640;    // size of the security camera picture
constexpr int kFeedHeight = 360;

void glfwErrorCallback(int code, const char* description)
{
    std::cerr << "[GLFW] Error " << code << ": " << description << "\n";
}

void printControls()
{
    std::cout << "Controls:\n"
                 "  left mouse drag   orbit the camera       right mouse drag  pan\n"
                 "  mouse wheel       zoom                   W A S D / Q E     move the camera target\n"
                 "  arrow keys        orbit with the keys    5 6 7 8 9 0       camera views\n"
                 "  1 2 3 4 / M       monitor arrangement    V                 camera feed on monitor 2\n"
                 "  L / F             desk lamp / ceiling    G                 gaming mode (lights out)\n"
                 "  R                 next RGB effect        T / N             time-lapse / day <-> night\n"
                 "  [ ]               time of day -/+        K                 spin the chair\n"
                 "  H                 hide / show the panel  Esc               quit\n";
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

    // Our callbacks first; ImGui chains to them when it installs its own.
    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, [](GLFWwindow* w, int k, int, int a, int m) { self(w)->onKey(k, a, m); });
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* w, int b, int a, int) { self(w)->onMouseButton(b, a); });
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* w, double x, double y) { self(w)->onCursor(x, y); });
    glfwSetScrollCallback(m_window, [](GLFWwindow* w, double, double dy) { self(w)->onScroll(dy); });
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* w, int x, int y) { self(w)->onResize(x, y); });
    glfwGetFramebufferSize(m_window, &m_fbWidth, &m_fbHeight);

    glEnable(GL_MULTISAMPLE);

    if (!m_renderer.init()) {
        std::cerr << "Shader loading failed - see messages above.\n";
        return false;
    }
    m_scene.init();
    m_feed.create(kFeedWidth, kFeedHeight);
    m_gui.init(m_window);
    printControls();
    return true;
}

void App::shutdown()
{
    if (!m_window) return;
    m_gui.shutdown();
    glfwDestroyWindow(m_window);
    glfwTerminate();
    m_window = nullptr;
}

void App::applyCameraView(int index)
{
    struct View { Vec3 target; float dist, yaw, pitch; };
    const View views[] = {
        {{0.05f, 0.95f, -1.85f}, 2.7f, 34.0f, 20.0f},  // overview
        {{-0.03f, 0.80f, -1.76f}, 0.62f, 12.0f, 38.0f}, // keyboard close-up
        {{0.0f, 1.10f, -2.10f}, 1.45f, 0.0f, 6.0f},     // monitors
        {{1.22f, 0.30f, -2.05f}, 1.1f, 22.0f, 14.0f},   // PC
        {{-2.4f, 1.45f, -1.2f}, 2.5f, 78.0f, 4.0f},     // window and plant
        {{0.0f, 1.0f, -0.6f}, 7.4f, 38.0f, 38.0f},      // whole room from outside (walls cut away)
    };
    if (index < 0 || index > 5) return;
    const View& v = views[index];
    m_camera.focus(v.target, v.dist, v.yaw, v.pitch);
}

// ---------------------------------------------------------------- input

void App::onKey(int key, int action, int mods)
{
    if (action != GLFW_PRESS) return;
    if (m_gui.wantsKeyboard()) return;
    SceneSettings& s = m_scene.settings;

    switch (key) {
    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(m_window, GLFW_TRUE); break;
    case GLFW_KEY_H: m_gui.visible = !m_gui.visible; break;
    case GLFW_KEY_0: applyCameraView(5); break;
    case GLFW_KEY_5: case GLFW_KEY_6: case GLFW_KEY_7: case GLFW_KEY_8: case GLFW_KEY_9:
        applyCameraView(key - GLFW_KEY_5);
        break;
    case GLFW_KEY_1: case GLFW_KEY_2: case GLFW_KEY_3: case GLFW_KEY_4:
        m_scene.applyArrangementPreset(static_cast<Arrangement>(key - GLFW_KEY_1));
        break;
    case GLFW_KEY_M: m_scene.nextArrangement(); break;
    case GLFW_KEY_L: s.deskLamp = !s.deskLamp; break;
    case GLFW_KEY_F: s.ceilingLight = !s.ceilingLight; break;
    case GLFW_KEY_G: s.gamingMode = !s.gamingMode; break;
    case GLFW_KEY_R: s.rgbMode = (s.rgbMode + ((mods & GLFW_MOD_SHIFT) ? kRgbModeCount - 1 : 1)) % kRgbModeCount; break;
    case GLFW_KEY_T: s.autoTime = !s.autoTime; break;
    case GLFW_KEY_N: s.timeOfDay = (s.timeOfDay > 6.5f && s.timeOfDay < 18.5f) ? 21.5f : 11.0f; break;
    case GLFW_KEY_V:
        s.content[1] = (s.content[1] == ScreenContent::CameraFeed) ? ScreenContent::Terminal : ScreenContent::CameraFeed;
        break;
    case GLFW_KEY_K: m_scene.spinChair(420.0f); break;
    default: break;
    }
}

void App::onMouseButton(int button, int action)
{
    bool down = action == GLFW_PRESS;
    if (down && m_gui.wantsMouse()) return;   // clicks on the panel do not move the camera
    if (button == GLFW_MOUSE_BUTTON_LEFT) m_leftDown = down;
    if (button == GLFW_MOUSE_BUTTON_RIGHT) m_rightDown = down;
    glfwGetCursorPos(m_window, &m_lastX, &m_lastY);
}

void App::onCursor(double x, double y)
{
    float dx = static_cast<float>(x - m_lastX);
    float dy = static_cast<float>(y - m_lastY);
    m_lastX = x;
    m_lastY = y;

    const float sensitivity = 0.25f;   // degrees per pixel
    if (m_leftDown) m_camera.orbit(-dx * sensitivity, dy * sensitivity);
    if (m_rightDown) {
        float k = 0.0012f * m_camera.distance;
        m_camera.pan(-dx * k, dy * k, 0.0f);
    }
}

void App::onScroll(double dy)
{
    if (m_gui.wantsMouse()) return;
    m_camera.zoom(static_cast<float>(dy));
}

void App::onResize(int width, int height)
{
    m_fbWidth = width;
    m_fbHeight = height;
}

void App::processInput(float dt)
{
    if (m_gui.wantsKeyboard()) return;
    auto down = [&](int key) { return glfwGetKey(m_window, key) == GLFW_PRESS; };

    // Every movement is multiplied by dt -> the same speed at any frame rate.
    float speed = (down(GLFW_KEY_LEFT_SHIFT) ? 3.0f : 1.0f) * dt;
    float right = 0.0f, up = 0.0f, forward = 0.0f;
    if (down(GLFW_KEY_W)) forward += 1.2f * speed;
    if (down(GLFW_KEY_S)) forward -= 1.2f * speed;
    if (down(GLFW_KEY_D)) right += 1.2f * speed;
    if (down(GLFW_KEY_A)) right -= 1.2f * speed;
    if (down(GLFW_KEY_E)) up += 1.2f * speed;
    if (down(GLFW_KEY_Q)) up -= 1.2f * speed;
    m_camera.pan(right, up, forward);

    float orbitSpeed = 70.0f * speed;   // degrees per second
    if (down(GLFW_KEY_LEFT)) m_camera.orbit(orbitSpeed, 0.0f);
    if (down(GLFW_KEY_RIGHT)) m_camera.orbit(-orbitSpeed, 0.0f);
    if (down(GLFW_KEY_UP)) m_camera.orbit(0.0f, orbitSpeed);
    if (down(GLFW_KEY_DOWN)) m_camera.orbit(0.0f, -orbitSpeed);

    SceneSettings& s = m_scene.settings;
    if (down(GLFW_KEY_LEFT_BRACKET)) s.timeOfDay -= 2.0f * dt;
    if (down(GLFW_KEY_RIGHT_BRACKET)) s.timeOfDay += 2.0f * dt;
    if (s.timeOfDay < 0.0f) s.timeOfDay += 24.0f;
    if (s.timeOfDay >= 24.0f) s.timeOfDay -= 24.0f;
}

// ---------------------------------------------------------------- frame

void App::renderFrame()
{
    // Pass 1: the security camera view into the framebuffer (only if a monitor shows it).
    // While this picture is drawn, the monitor that shows it is dark, because a
    // texture cannot be read and written in the same pass.
    if (m_scene.feedNeeded()) {
        m_scene.setFeedTexture(0);
        m_feed.bind();
        glClearColor(0.02f, 0.02f, 0.03f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderView feed;
        feed.eye = m_scene.feedEye();
        feed.view = m_scene.feedView();
        feed.projection = perspective(m_scene.feedFovDeg(), static_cast<float>(kFeedWidth) / kFeedHeight, 0.05f, 30.0f);
        m_renderer.render(m_scene, feed);
        m_scene.setFeedTexture(m_feed.colorTexture());   // now the monitor shows the picture
    }

    // Pass 2: the main camera to the window.
    Framebuffer::bindDefault(m_fbWidth, m_fbHeight);
    glClearColor(0.07f, 0.07f, 0.085f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderView main;
    main.eye = m_camera.position();
    main.view = m_camera.view();
    main.projection = m_camera.projection(static_cast<float>(m_fbWidth) / std::max(1, m_fbHeight));
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

        processInput(dt);
        m_camera.update(dt);
        m_scene.update(dt, static_cast<float>(now));

        if (m_fbWidth > 0 && m_fbHeight > 0) {
            renderFrame();

            m_gui.beginFrame();
            GuiActions a = m_gui.draw(m_scene, m_fps);
            m_gui.endFrame();
            if (a.resetCamera) m_camera.reset();
            if (a.cameraView >= 0) applyCameraView(a.cameraView);
        }
        glfwSwapBuffers(m_window);
    }
    shutdown();
    return 0;
}
