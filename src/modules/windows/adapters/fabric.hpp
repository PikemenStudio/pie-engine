//
// Created by FullHat on 01/04/2024.
//

#ifndef ENGINE_SRC_MODULES_WINDOWS_ADAPTERS_FABRIC_HPP
#define ENGINE_SRC_MODULES_WINDOWS_ADAPTERS_FABRIC_HPP

#include "../../../adapters_interfaces/fabric_adapter.hpp"
#include "../src/window.hpp"
#include <memory>

class WindowAdapter : public Adapter<windows::Window, windows::Window::WindowProps> {
  using InitParams = windows::Window::WindowProps;
  using Target = windows::Window;
public:
  WindowAdapter(InitParams Params){Obj = std::make_unique<Target>(Params);}

  std::vector<const char *> getRequiredExtensions() const { return Obj->getRequiredExtensions(); }
};

#endif // ENGINE_SRC_MODULES_WINDOWS_ADAPTERS_FABRIC_HPP
