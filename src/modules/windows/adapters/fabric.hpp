//
// Created by FullHat on 01/04/2024.
//

#ifndef ENGINE_SRC_MODULES_WINDOWS_ADAPTERS_FABRIC_HPP
#define ENGINE_SRC_MODULES_WINDOWS_ADAPTERS_FABRIC_HPP

#include "../../../adapters_interfaces/fabric_adapter.hpp"
#include "../src/window.h"
#include <memory>

template <typename T>
concept WindowAdapterMethods = requires(T t) {
  { t.test() } -> std::same_as<void>;
};

template <WindowAdapterMethods T = windows::Window,
          typename InitParams = windows::Window::WindowProps>
class WindowAdapter : public Adapter<T, InitParams> {
public:
  WindowAdapter(InitParams Params){Adapter<T, InitParams>::Obj = std::make_unique<T>(Params);}
  IMPL_FUNC(void, test)
};

#endif // ENGINE_SRC_MODULES_WINDOWS_ADAPTERS_FABRIC_HPP
