// Gui.h - Dear ImGui control panel: on-screen help, FPS, and controls for the
// items, colours, lights, RGB, monitors and the camera views.
#pragma once

struct GLFWwindow;
class DeskScene;
class FpsCounter;

// Things the panel asks the application to do this frame.
struct GuiActions {
    bool resetCamera = false;
    int cameraView = -1;   // 0..5 = camera view
};

class Gui {
public:
    bool init(GLFWwindow* window);
    void shutdown();

    void beginFrame();
    GuiActions draw(DeskScene& scene, const FpsCounter& fps);
    void endFrame();

    bool wantsMouse() const;
    bool wantsKeyboard() const;

    bool visible = true;

private:
    bool m_initialized = false;
};
