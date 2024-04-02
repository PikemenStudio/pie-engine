//
// Created by FullHat on 30/03/2024.
//

#ifndef ENGINE_SRC_MODULES_WINDOWS_SRC_WINDOW_H
#define ENGINE_SRC_MODULES_WINDOWS_SRC_WINDOW_H

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <memory>
#include <string>

namespace windows {

class Window {
public:
  struct WindowProps;

  Window(WindowProps Props);
  ~Window() = default;

public:
  // Member structs
  struct WindowProps {
    glm::vec2 Size{200, 200};
    std::string Title;

    enum WindowMode { WINDOWED, BORDERLESS, FULLSCREEN } Mode = WINDOWED;

    // Used only if Mode == WINDOWED
    bool IsResizable{true};
  };

  void test() {}

  operator std::shared_ptr<GLFWwindow>() const;

protected:
  // Build glfw object
  void buildWindow();

  // Members
protected:
  WindowProps Props;

  std::shared_ptr<GLFWwindow> GlfwWindow;
};

} // namespace windows

#endif // ENGINE_SRC_MODULES_WINDOWS_SRC_WINDOW_H