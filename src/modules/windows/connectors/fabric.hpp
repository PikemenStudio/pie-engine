//
// Created by FullHat on 01/04/2024.
//

#ifndef ENGINE_SRC_MODULES_WINDOWS_CONNECTORS_FABRIC_HPP
#define ENGINE_SRC_MODULES_WINDOWS_CONNECTORS_FABRIC_HPP

#include "../src/window.h"
#include <memory>

class WindowFabric {
public:
  static std::unique_ptr<windows::Window>
  createWindow(windows::Window::WindowProps Props) {
    return std::make_unique<windows::Window>(Props);
  }
};

#endif // ENGINE_SRC_MODULES_WINDOWS_CONNECTORS_FABRIC_HPP
