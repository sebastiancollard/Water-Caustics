#pragma once
#include "glad/glad.h"
//
#include <GLFW/glfw3.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// io
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "command_map.h"
#include "panel.h"

namespace giv {
namespace io {

//
// Keyboard
//
struct KeyboardEvent {
  int key;
  int action;
  int mods;
};

using KeyboardCommands = Commands<int, KeyboardEvent>;
using Key = KeyboardCommands::command_type;

//
// Mouse
//
struct MouseButtonEvent {
  int button;
  int action;
  int mods;
};

using MouseCommands = Commands<int, MouseButtonEvent>;
using MouseButton = MouseCommands::command_type;

//
// Cursor
//
struct CursorPosition {
  double x;
  double y;
};
using CursorCommand = std::function<void(CursorPosition event)>;

//
// Scroll
//
struct ScrollWheelEvent {
  double xOffset;
  double yOffset;
};
using ScrollWheelCommand = std::function<void(ScrollWheelEvent event)>;

//
//   GLFW Window handle
//
class WindowHandle final {
public:
  using destroyFunction = void (*)(GLFWwindow *);
  using type = std::unique_ptr<GLFWwindow, destroyFunction>;

  explicit WindowHandle(GLFWwindow *window);

  operator GLFWwindow *();

  // TODO FIX CONST CORRECTNESS of return
  operator GLFWwindow *() const;
  GLFWwindow *data() const;
  GLFWwindow *data();

  static void destroy(GLFWwindow *window);

  friend void swap(WindowHandle &, WindowHandle &);

private:
  type m_handle;
};

//
//    Window
//
struct dimensions {
  uint32_t width;
  uint32_t height;
};

struct Properties {
  uint32_t width() const;
  uint32_t height() const;
  dimensions size() const;
  dimensions frameBufferSize() const;
  std::string title() const;
  std::string glslVersionString() const;
  bool isVSyncEnabled() const;

  Properties &width(uint32_t width);
  Properties &height(uint32_t height);
  Properties &size(dimensions dimensions);
  Properties &frameBufferSize(dimensions dimensions);
  Properties &title(std::string title);
  Properties &glslVersionString(std::string title);
  Properties &enableVSync(bool use);

private: // variables
  dimensions m_frameBufferSize;
  dimensions m_dimensions;
  std::string m_title;
  std::string m_glslVersionString;
  bool m_enableVSync = false;
};

class Window : private Properties {
public: // functions
  explicit Window(WindowHandle handle);
  Window(WindowHandle handle, Properties properties);

  virtual ~Window() = default;

  // need to explicitly instantiate them, because virtual
  // glfwWindowUserPointNeeds to be reset
  Window(Window &&);
  Window &operator=(Window &&);

  // todo (aro) remove to properties
  void enableVsync(bool use);
  void shouldClose();

  using Properties::frameBufferSize;
  using Properties::glslVersionString;
  using Properties::height;
  using Properties::isVSyncEnabled;
  using Properties::size;
  using Properties::title;
  using Properties::width;

  KeyboardCommands &keyboardCommands();
  MouseCommands &mouseCommands();
  CursorCommand &cursorCommand();
  ScrollWheelCommand &scrollWheelCommand();

  WindowHandle const &handle() const;
  WindowHandle &handle();

  void windowResizeEvent(dimensions event);
  void framebufferResizeEvent(dimensions event);

  virtual void keyboardEvent(KeyboardEvent event);
  virtual void mouseButtonEvent(MouseButtonEvent event);
  virtual void cursorMovementEvent(CursorPosition event);
  virtual void scrollWheelEvent(ScrollWheelEvent event);

private: // functions
  friend class GLFWContext;
  friend class ImGuiContext;

  void init();
  void setCallbacks();

private: // variables
  WindowHandle m_handle;

  KeyboardCommands m_keyboardCommands;
  MouseCommands m_mouseCommands;
  CursorCommand m_cursorCommand = [](CursorPosition) {};
  ScrollWheelCommand m_scrollWheelCommand = [](ScrollWheelEvent) {};
};

//
// ImGui Context
//
class ImGuiContext final {
public:
  ImGuiContext(WindowHandle const &windowHandle, char const *glslVersion);
  ~ImGuiContext();

  ImGuiContext(ImGuiContext const &) = delete;
  ImGuiContext &operator=(ImGuiContext const &) = delete;
};

void ImGuiBeginFrame();

void ImGuiEndFrame();

void ImGuiRenderDrawData();

//
// ImGui Window
//
class ImGuiWindow final : public Window {
public:
  explicit ImGuiWindow(WindowHandle handle);
  ImGuiWindow(WindowHandle handle, giv::io::Properties properties);

  // need to explicitly instantiate them, because virtual
  // glfwWindowUserPointNeeds to be reset
  ImGuiWindow(ImGuiWindow &&);
  ImGuiWindow &operator=(ImGuiWindow &&);

  void keyboardEvent(KeyboardEvent event) override;
  void mouseButtonEvent(MouseButtonEvent event) override;
  void cursorMovementEvent(CursorPosition event) override;
  void scrollWheelEvent(ScrollWheelEvent event) override;

private:
private:
  std::unique_ptr<ImGuiContext> m_ImGuiContext;
};

//
//    GLFW
//
GLFWwindow *createWindow(Properties const &properties);

class GLFWContext final {
public: // interface
  GLFWContext();
  ~GLFWContext();

  Window makeWindow(Properties properties);
  ImGuiWindow makeImGuiWindow(Properties properties);

  void makeCurrent(Window const &window);

  GLFWContext &glMajorVesion(int version);
  GLFWContext &glMinorVesion(int version);
  GLFWContext &glForwardComaptability(bool value);
  GLFWContext &glCoreProfile();
  GLFWContext &glAntiAliasingSamples(int samples);
  GLFWContext &matchPrimaryMonitorVideoMode();

private: // functions
  bool initalize();
  void shutdown();
};

std::string glfwVersionString();

void preRender(Window const &);
void preRender(ImGuiWindow const &);

void postRender(Window const &window);
void postRender(ImGuiWindow const &window);

template <typename Window_t, typename Func>
void mainloop(Window_t &&window, Func function) {
  auto previous_time = glfwGetTime();
  while (!glfwWindowShouldClose(window.handle())) {

    auto current_time = glfwGetTime();
    auto delta_t = current_time - previous_time;
    previous_time = current_time;

    preRender(window);

    function(float(delta_t));

    postRender(window);
  }
}

} // namespace io
} // namespace giv
