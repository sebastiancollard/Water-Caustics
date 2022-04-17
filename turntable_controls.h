#pragma once
#include "givio.h"

#include "givr.h"

template <typename CameraT> class TurnTableControls {
public:
  TurnTableControls(giv::io::Window &w, CameraT &c) : m_window(w), m_camera(c) {
    m_window.keyboardCommands() |
        giv::io::Key(GLFW_KEY_ESCAPE,
                     [&](auto const & /*event*/) { m_window.shouldClose(); }) |
        giv::io::Key(GLFW_KEY_X,
                     [&](auto const &event) {
                       if (event.action == GLFW_PRESS)
                         xLock = !xLock;
                     }) |
        giv::io::Key(GLFW_KEY_Y, [&](auto const &event) {
          if (event.action == GLFW_PRESS)
            yLock = !yLock;
        });

    m_window.mouseCommands() | //
        giv::io::MouseButton(  //
            GLFW_MOUSE_BUTTON_LEFT, [&](auto const &event) {
              if (event.action == GLFW_PRESS) {
                cursorLocked = true;
              } else {
                cursorLocked = false;
              }

              if (event.mods == GLFW_MOD_SHIFT) {
                cursorZoom = true;
              } else {
                cursorZoom = false;
              }
              if (event.mods == GLFW_MOD_CONTROL) {
                cursorPan = true;
              } else {
                cursorPan = false;
              }
            });

    m_window.scrollWheelCommand() = [&](auto event) {
      m_camera.zoom(event.yOffset);
    };

    m_window.cursorCommand() = [&](auto const &event) {
      if (cursorLocked) {
        if (cursorZoom) {
          float deltaY = (cursorPosition.y - event.y);
          float percY = deltaY / m_window.height();
          m_camera.zoom(-percY * 50.f);
        } else if (cursorPan) {
          float deltaX = 0.25f * (cursorPosition.x - event.x);
          float deltaY = 0.25f * (cursorPosition.y - event.y);
          m_camera.translate(givr::vec3f(deltaX, -deltaY, 0.f));
        } else {
          float deltaX = cursorPosition.x - event.x;
          float deltaY = cursorPosition.y - event.y;
          float percX = deltaX / m_window.width();
          float percY = deltaY / m_window.height();
          if (!yLock)
            m_camera.rotateAroundXPercent(percX);
          if (!xLock)
            m_camera.rotateAroundYPercent(percY);
        }
      }
      cursorPosition = event;
    };
  }

private:
  giv::io::Window &m_window;
  CameraT &m_camera;
  giv::io::CursorPosition cursorPosition;
  bool cursorLocked = false;
  bool cursorZoom = false;
  bool cursorPan = false;
  bool xLock = false;
  bool yLock = false;
};
