//
// Created by FullHat on 30/03/2024.
//

#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include "glm/glm.hpp"
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
    glm::vec2 Size {200, 200};
    std::string Title;

    enum WindowMode { WINDOWED, BORDERLESS, FULLSCREEN } Mode = WINDOWED;

    // Used only if Mode == WINDOWED
    bool IsResizable { true };
  };

  void test() {}

  // Members
protected:
  WindowProps Props;
};

} // namespace windows

#endif // ENGINE_WINDOW_H
