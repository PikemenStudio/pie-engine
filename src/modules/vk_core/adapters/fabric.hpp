//
// Created by FullHat on 02/04/2024.
//

#ifndef ENGINE_SRC_MODULES_VK_CORE_ADAPTERS_FABRIC_HPP
#define ENGINE_SRC_MODULES_VK_CORE_ADAPTERS_FABRIC_HPP

#include "../../../adapters_interfaces/fabric_adapter.hpp"

template <typename T>
concept GraphicApiAdapterMethods = requires(T t) {
  { t.test() } -> std::same_as<void>;
};

//template <GraphicApiAdapterMethods T = windows::Window,
//          typename InitParams = windows::Window::WindowProps>
//class GraphicApiAdapter : public Adapter<T, InitParams> {
//public:
//  GraphicApiAdapter(InitParams Params){Adapter<T, InitParams>::Obj = std::make_unique<T>(Params);}
//  IMPL_FUNC(void, test)
//};

#endif // ENGINE_SRC_MODULES_VK_CORE_ADAPTERS_FABRIC_HPP
