#pragma once

#include <iosfwd>
#include <string>

#include "givio.h"
#include "givr.h"
#include "imgui/imgui.h"

namespace panel {

extern bool showPanel;
extern ImVec4 clear_color;

// animation
extern bool playModel;
extern bool resetModel;
extern bool stepModel;
extern float dt;
extern float kSeperation;
extern float kCohesion;
extern float kAllignment;
extern int numberOfBoid;
extern int numberOfSphere;

// reset
extern bool resetView;

void updateMenu();

} // namespace panel
