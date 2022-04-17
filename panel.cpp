#include "panel.h"

#include <array>

namespace panel {

// default values
bool showPanel = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// animation
bool playModel = false;
bool resetModel = false;
bool stepModel = false;
float dt = 0.002f;

float kSeperation = 100.0f;
float kCohesion = 8.0f;
float kAllignment = 10.0f;

int numberOfBoid = 500;
int numberOfSphere = 20;

// reset
bool resetView = false;

void updateMenu() {
  using namespace ImGui;

  giv::io::ImGuiBeginFrame();

  if (showPanel && Begin("panel", &showPanel, ImGuiWindowFlags_MenuBar)) {
    if (BeginMenuBar()) {
      if (BeginMenu("File")) {
        if (MenuItem("Close", "(P)")) {
          showPanel = false;
        }
        // add more if you would like...
        ImGui::EndMenu();
      }
      EndMenuBar();
    }

    Spacing();
    if (CollapsingHeader("Background Color")) { // Clear
      ColorEdit3("Clear color", (float *)&clear_color);
    }

    Spacing();
    Separator();
    if (Button("Play/Pause")) {
      playModel = !playModel;
    }
    resetModel = Button("Reset Model");
    stepModel = Button("Step");
    InputFloat("dt", &dt, 0.00001f, 0.1f, "%.5f");
    SliderFloat("kSeperation", &kSeperation, 0.0f, 1000.0f, "%.2f");
    SliderFloat("kAllignment", &kAllignment, 0.0f, 1000.0f, "%.2f");
    SliderFloat("kCohesion", &kCohesion, 0.0f, 1000.0f, "%.2f");
    SliderInt("Number Of Boids", &numberOfBoid, 0, 2000, "%d");
    SliderInt("Number Of Spheres", &numberOfSphere, 0, 2000, "%d");

    Spacing();
    Separator();
    resetView = Button("Reset view");

    Spacing();
    Separator();
    Text("Application average %.3f ms/frame (%.1f FPS)",
         1000.0f / GetIO().Framerate, GetIO().Framerate);

    End();
  }
  giv::io::ImGuiEndFrame();
}

} // namespace panel
