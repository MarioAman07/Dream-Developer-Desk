#include "ui/Gui.h"

#include "core/FpsCounter.h"
#include "scene/DeskScene.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstdio>

namespace {
void helpLine(const char* keys, const char* what)
{
    ImGui::TextColored(ImVec4(1.0f, 0.78f, 0.35f, 1.0f), "%-12s", keys);
    ImGui::SameLine(110);
    ImGui::TextUnformatted(what);
}
} // namespace

bool Gui::init(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;   // do not write imgui.ini next to the exe

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    m_initialized = true;
    return true;
}

void Gui::shutdown()
{
    if (!m_initialized) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    m_initialized = false;
}

void Gui::beginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::endFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool Gui::wantsMouse() const { return visible && ImGui::GetIO().WantCaptureMouse; }
bool Gui::wantsKeyboard() const { return visible && ImGui::GetIO().WantCaptureKeyboard; }

GuiActions Gui::draw(DeskScene& scene, const FpsCounter& fps)
{
    GuiActions actions;
    if (!visible) return actions;

    SceneSettings& s = scene.settings;
    ImGui::SetNextWindowPos(ImVec2(12, 12), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(350, 0), ImGuiCond_FirstUseEver);
    ImGui::Begin("Dream Developer Desk", &visible, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%.0f FPS (%.2f ms per frame)", fps.fps, fps.ms);

    if (ImGui::CollapsingHeader("Controls (help)", ImGuiTreeNodeFlags_DefaultOpen)) {
        helpLine("LMB drag", "orbit the camera");
        helpLine("RMB drag", "pan the orbit target");
        helpLine("Wheel", "zoom");
        helpLine("WASD QE", "move the camera target");
        helpLine("Arrows", "orbit with keys");
        helpLine("5 6 7 8 9 0", "camera views (0 = whole room)");
        helpLine("1 2 3 4 / M", "monitor arrangement");
        helpLine("L / F", "desk lamp / ceiling light");
        helpLine("G", "gaming mode (lights out)");
        helpLine("R", "next RGB effect");
        helpLine("T / N", "time-lapse / day <-> night");
        helpLine("[ ]", "time of day -/+");
        helpLine("V", "camera feed on monitor 2");
        helpLine("K", "spin the chair");
        helpLine("H / Esc", "hide this panel / quit");
    }

    if (ImGui::CollapsingHeader("Lighting & time")) {
        ImGui::SliderFloat("Time of day", &s.timeOfDay, 0.0f, 23.99f, "%.1f h");
        ImGui::Checkbox("Time-lapse", &s.autoTime);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120);
        ImGui::SliderFloat("h/sec", &s.timeSpeed, 0.1f, 4.0f, "%.1f");
        ImGui::Checkbox("Ceiling light", &s.ceilingLight);
        ImGui::SameLine();
        ImGui::Checkbox("Desk lamp", &s.deskLamp);
        ImGui::Checkbox("GAMING MODE (lights out, RGB on)", &s.gamingMode);
    }

    if (ImGui::CollapsingHeader("RGB")) {
        ImGui::Combo("Effect", &s.rgbMode, kRgbModeNames, kRgbModeCount);
        ImGui::SliderFloat("Speed", &s.rgbSpeed, 0.1f, 4.0f);
        ImGui::SliderFloat("Brightness", &s.rgbBrightness, 0.0f, 2.0f);
        ImGui::ColorEdit3("Static colour", &s.rgbStatic.x);
    }

    if (ImGui::CollapsingHeader("Monitors")) {
        int arr = static_cast<int>(s.arrangement);
        if (ImGui::Combo("Arrangement", &arr, kArrangementNames, static_cast<int>(Arrangement::Count)))
            scene.applyArrangementPreset(static_cast<Arrangement>(arr));
        for (int i = 0; i < MonitorRig::kCount; ++i) {
            char label[32];
            std::snprintf(label, sizeof(label), "Monitor %d", i + 1);
            int c = static_cast<int>(s.content[i]);
            if (ImGui::Combo(label, &c, kScreenContentNames, static_cast<int>(ScreenContent::Count)))
                s.content[i] = static_cast<ScreenContent>(c);
        }
        ImGui::SliderFloat("Screen brightness", &s.screenBrightness, 0.2f, 2.0f);
    }

    if (ImGui::CollapsingHeader("Items & colours")) {
        ImGui::Checkbox("Plant", &s.showPlant);        ImGui::SameLine(120);
        ImGui::Checkbox("Mug", &s.showMug);            ImGui::SameLine(230);
        ImGui::Checkbox("PC", &s.showPC);
        ImGui::Checkbox("Chair", &s.showChair);        ImGui::SameLine(120);
        ImGui::Checkbox("Lamp", &s.showLamp);          ImGui::SameLine(230);
        ImGui::Checkbox("Shelf", &s.showShelf);
        ImGui::Checkbox("Poster", &s.showPoster);      ImGui::SameLine(120);
        ImGui::Checkbox("Rug", &s.showRug);            ImGui::SameLine(230);
        ImGui::Checkbox("Sec. cam", &s.showSecurityCam);
        ImGui::ColorEdit3("Desk tint", &s.deskTint.x);
        ImGui::ColorEdit3("Wall colour", &s.wallColor.x);
        ImGui::ColorEdit3("Chair colour", &s.chairColor.x);   // not "Chair": that is the checkbox
        ImGui::ColorEdit3("Chair accent", &s.chairAccent.x);
        ImGui::SliderFloat("Chair turn", &s.chairYaw, -180.0f, 180.0f, "%.0f deg");
    }

    if (ImGui::CollapsingHeader("Desk lamp (hierarchy)")) {
        ImGui::SliderFloat("Base yaw", &s.lampYaw, 40.0f, 170.0f, "%.0f deg");
        ImGui::SliderFloat("Lower arm", &s.lampLowerArm, -10.0f, 60.0f, "%.0f deg");
        ImGui::SliderFloat("Upper arm", &s.lampUpperArm, 20.0f, 130.0f, "%.0f deg");
        ImGui::SliderFloat("Head", &s.lampHead, -170.0f, -60.0f, "%.0f deg");
        ImGui::ColorEdit3("Light colour", &s.lampColor.x);
        ImGui::SliderFloat("Intensity", &s.lampIntensity, 0.0f, 4.0f);
    }

    if (ImGui::CollapsingHeader("Camera")) {
        // ImGui identifies a widget by its label. "Monitors" and "PC" are also used by
        // the "Monitors" header and the "PC" checkbox, so the buttons get their own
        // ID scope - otherwise the clicks go to the wrong widget.
        const char* views[] = {"Overview", "Keyboard", "Monitors", "PC", "Window", "Room"};
        ImGui::PushID("camera views");
        for (int i = 0; i < 6; ++i) {
            if (i) ImGui::SameLine();
            if (ImGui::SmallButton(views[i])) actions.cameraView = i;
        }
        ImGui::PopID();
        if (ImGui::Button("Reset camera")) actions.resetCamera = true;
    }

    ImGui::End();
    return actions;
}
