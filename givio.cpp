#include "givio.h"

namespace giv {
namespace io {

//
// Window handle
WindowHandle::WindowHandle(GLFWwindow *window) : m_handle(window, destroy) {}

WindowHandle::operator GLFWwindow *() { return m_handle.get(); }

WindowHandle::operator GLFWwindow *() const { return m_handle.get(); }

GLFWwindow *WindowHandle::data() const { return m_handle.get(); }

GLFWwindow *WindowHandle::data() { return m_handle.get(); }

void WindowHandle::destroy(GLFWwindow *window) { glfwDestroyWindow(window); }

void swap(WindowHandle &lhs, WindowHandle &rhs) {
  std::swap(lhs.m_handle, rhs.m_handle);
}

//
// Window properties
//
dimensions Properties::size() const { return m_dimensions; }

dimensions Properties::frameBufferSize() const { return m_frameBufferSize; }

uint32_t Properties::width() const { return m_dimensions.width; }

uint32_t Properties::height() const { return m_dimensions.height; }

std::string Properties::title() const { return m_title; }

std::string Properties::glslVersionString() const {
  return m_glslVersionString;
}

bool Properties::isVSyncEnabled() const { return m_enableVSync; }

Properties &Properties::title(std::string title) {
  m_title = title;
  return *this;
}

Properties &Properties::size(dimensions dimensions) {
  m_dimensions = dimensions;
  return *this;
}

Properties &Properties::frameBufferSize(dimensions dimensions) {
  m_frameBufferSize = dimensions;
  return *this;
}

Properties &Properties::width(uint32_t width) {
  m_dimensions.width = width;
  return *this;
}

Properties &Properties::height(uint32_t height) {
  m_dimensions.height = height;
  return *this;
}

Properties &Properties::glslVersionString(std::string versionString) {
  m_glslVersionString = versionString;
  return *this;
}

Properties &Properties::enableVSync(bool use) {
  m_enableVSync = use;
  return *this;
}

//
// Window
//
Window::Window(WindowHandle handle) : m_handle(std::move(handle)) { init(); }

Window::Window(WindowHandle handle, Properties properties)
    : Properties(properties), m_handle(std::move(handle)) {
  init();
}

Window::Window(Window &&other)
    : Properties(std::move(other)), m_handle(nullptr) {
  std::swap(m_handle, other.m_handle);
  std::swap(m_keyboardCommands, other.m_keyboardCommands);
  std::swap(m_mouseCommands, other.m_mouseCommands);
  std::swap(m_cursorCommand, other.m_cursorCommand);
  std::swap(m_scrollWheelCommand, other.m_scrollWheelCommand);

  init();
}

Window &Window::operator=(Window &&other) {
  if (this != &other) {
    Properties::operator=(std::move(other));
    m_handle = std::move(other.m_handle);
    std::swap(m_keyboardCommands, other.m_keyboardCommands);
    std::swap(m_mouseCommands, other.m_mouseCommands);
    std::swap(m_cursorCommand, other.m_cursorCommand);
    std::swap(m_scrollWheelCommand, other.m_scrollWheelCommand);

    init();
  }
  return *this;
}

void Window::enableVsync(bool use) {
  Properties::enableVSync(use);

  if (isVSyncEnabled())
    glfwSwapInterval(1); // frames between
  else
    glfwSwapInterval(0); // 0 frames between
}

void Window::shouldClose() { glfwSetWindowShouldClose(handle(), GLFW_TRUE); }

void Window::windowResizeEvent(dimensions event) { size(event); }

void Window::framebufferResizeEvent(dimensions event) {
  frameBufferSize(event);
  glViewport(0, 0, GLsizei(event.width), GLsizei(event.height));
}

void Window::keyboardEvent(KeyboardEvent event) {
  m_keyboardCommands.execute(event.key, std::move(event));
}

void Window::mouseButtonEvent(MouseButtonEvent event) {
  m_mouseCommands.execute(event.button, std::move(event));
}

void Window::cursorMovementEvent(CursorPosition event) {
  m_cursorCommand(std::move(event));
}

void Window::scrollWheelEvent(ScrollWheelEvent event) {
  m_scrollWheelCommand(std::move(event));
}

// *.cpp helper function (inlined most likely)
Window *userPointer(GLFWwindow *window) {
  return static_cast<Window *>(glfwGetWindowUserPointer(window));
}

void Window::setCallbacks() {
  // Window size
  glfwSetWindowSizeCallback(
      handle(), [](GLFWwindow *window, int width, int height) {
        auto *windowData = userPointer(window);
        windowData->windowResizeEvent({uint32_t(width), uint32_t(height)});
      });

  glfwSetFramebufferSizeCallback(
      handle(), [](GLFWwindow *window, int width, int height) {
        auto *windowData = userPointer(window);
        windowData->framebufferResizeEvent({uint32_t(width), uint32_t(height)});
      });

  // Keyboard
  glfwSetKeyCallback(handle(), [](GLFWwindow *window, int key, int /*scancode*/,
                                  int action, int mods) {
    auto *windowData = userPointer(window);
    windowData->keyboardEvent({key, action, mods});
  });

  // Mouse
  glfwSetMouseButtonCallback(
      handle(), [](GLFWwindow *window, int button, int action, int mods) {
        auto *windowData = userPointer(window);
        windowData->mouseButtonEvent({button, action, mods});
      });

  // Cursor
  glfwSetCursorPosCallback(handle(),
                           [](GLFWwindow *window, double x, double y) {
                             auto *windowData = userPointer(window);
                             windowData->cursorMovementEvent({x, y});
                           });
  // Scroll
  glfwSetScrollCallback(handle(),
                        [](GLFWwindow *window, double xOffset, double yOffset) {
                          auto *windowData = userPointer(window);
                          windowData->scrollWheelEvent({xOffset, yOffset});
                        });
}

KeyboardCommands &Window::keyboardCommands() { return m_keyboardCommands; }

MouseCommands &Window::mouseCommands() { return m_mouseCommands; }

CursorCommand &Window::cursorCommand() { return m_cursorCommand; }

ScrollWheelCommand &Window::scrollWheelCommand() {
  return m_scrollWheelCommand;
}

WindowHandle const &Window::handle() const { return m_handle; }

WindowHandle &Window::handle() { return m_handle; }

void Window::init() {
  glfwSetWindowUserPointer(handle(), this);
  setCallbacks();
}

GLFWwindow *createWindow(Properties const &properties) {
  return glfwCreateWindow(properties.width(),         //
                          properties.height(),        //
                          properties.title().c_str(), //
                          nullptr,                    //
                          nullptr);
}

//
// GLFW Context
//
GLFWContext::GLFWContext() {
  if (!initalize())
    throw;
}

GLFWContext::~GLFWContext() { shutdown(); }

Window GLFWContext::makeWindow(Properties properties) {
  auto w = Window(WindowHandle(createWindow(properties)), properties);
  makeCurrent(w);
  return w;
}

ImGuiWindow GLFWContext::makeImGuiWindow(Properties properties) {
  // TODO need to make current here, and gladLoad before ImGui gets paws on
  // GLFWWindow *
  auto handle = WindowHandle(createWindow(properties));
  glfwMakeContextCurrent(handle);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  auto w = ImGuiWindow(std::move(handle), properties);
  makeCurrent(w);
  return w;
}

void GLFWContext::makeCurrent(Window const &window) {
  glfwMakeContextCurrent(window.handle());
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

GLFWContext &GLFWContext::glMajorVesion(int version) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version);
  return *this;
}

GLFWContext &GLFWContext::glMinorVesion(int version) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version);
  return *this;
}

GLFWContext &GLFWContext::glForwardComaptability(bool value) {
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, value); // true, requied on Mac
  return *this;
}

GLFWContext &GLFWContext::glCoreProfile() {
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2 + only
  return *this;
}
GLFWContext &GLFWContext::glAntiAliasingSamples(int samples) {
  glfwWindowHint(GLFW_SAMPLES, samples);
  return *this;
}

GLFWContext &GLFWContext::matchPrimaryMonitorVideoMode() {
  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
  return *this;
}

bool GLFWContext::initalize() { return glfwInit(); }

void GLFWContext::shutdown() { glfwTerminate(); }

std::string glfwVersionString() {
  int glfwMajor, glfwMinor, glfwRevision;
  glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);

  std::ostringstream oss;
  oss << "GLFW Version: " << glfwMajor << '.' << glfwMinor << '.'
      << glfwRevision;
  return oss.str();
}

//
// ImGui Context
//
ImGuiContext::ImGuiContext(WindowHandle const &windowHandle,
                           char const *glslVersion) {
  ImGui::CreateContext();
  // enable docking
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport /
                                                      // Platform Windows
  io.ConfigDockingWithShift = false;

  // true: install callbacks from window
  ImGui_ImplGlfw_InitForOpenGL(windowHandle, true);
  ImGui_ImplOpenGL3_Init(glslVersion);
  ImGui::StyleColorsDark();
}

ImGuiContext::~ImGuiContext() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void ImGuiBeginFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImGuiEndFrame() { ImGui::EndFrame(); }

void ImGuiRenderDrawData() {
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow *backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}

//
// ImGuiWindow
//
ImGuiWindow::ImGuiWindow(WindowHandle windowHandle)
    : Window(std::move(windowHandle)) {
  m_ImGuiContext =
      std::make_unique<ImGuiContext>(handle(), glslVersionString().c_str());
}

ImGuiWindow::ImGuiWindow(WindowHandle windowHandle,
                         giv::io::Properties properties)
    : Window(std::move(windowHandle), properties) {
  m_ImGuiContext =
      std::make_unique<ImGuiContext>(handle(), glslVersionString().c_str());
}

ImGuiWindow::ImGuiWindow(ImGuiWindow &&other) : Window(std::move(other)) {
  std::swap(m_ImGuiContext, other.m_ImGuiContext);
}

ImGuiWindow &ImGuiWindow::operator=(ImGuiWindow &&other) {
  if (this != &other) {
    Window::operator=(std::move(other));
    std::swap(m_ImGuiContext, other.m_ImGuiContext);
  }
  return *this;
}

void ImGuiWindow::keyboardEvent(KeyboardEvent event) {
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  if (io.WantCaptureKeyboard)
    return;

  keyboardCommands().execute(event.key, std::move(event));
}

void ImGuiWindow::mouseButtonEvent(MouseButtonEvent event) {
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  if (io.WantCaptureMouse)
    return;

  mouseCommands().execute(event.button, std::move(event));
}

void ImGuiWindow::cursorMovementEvent(CursorPosition event) {
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  if (io.WantCaptureMouse)
    return;

  cursorCommand()(std::move(event));
}

void ImGuiWindow::scrollWheelEvent(ScrollWheelEvent event) {
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  if (io.WantCaptureMouse)
    return;

  scrollWheelCommand()(std::move(event));
}

//
// pre/post render functions
//

void preRender(Window const &window) {}

void preRender(ImGuiWindow const &window) { panel::updateMenu(); }

void postRender(Window const &window) {
  glfwPollEvents();
  glfwSwapBuffers(window.handle());
}

void postRender(ImGuiWindow const &window) {
  ImGui::Render();
  glfwPollEvents();
  giv::io::ImGuiRenderDrawData();
  glfwSwapBuffers(window.handle());
}

} // namespace io
} // namespace giv
