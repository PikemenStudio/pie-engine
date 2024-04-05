//
// Created by FullHat on 02/04/2024.
//

#ifndef ENGINE_SRC_MODULES_VK_CORE_ADAPTERS_FABRIC_HPP
#define ENGINE_SRC_MODULES_VK_CORE_ADAPTERS_FABRIC_HPP

#include "../../../adapters_interfaces/fabric_adapter.hpp"
#include "../src/GraphicEngine.hpp"

template <typename T>
concept GraphicApiAdapterMethods = requires(T t) {
  { t.test() } -> std::same_as<void>;
};

class GraphicApiAdapter : public Adapter<vk_core::GraphicEngine, vk_core::GraphicEngine::GraphicEngineProps> {
  using InitParams = vk_core::GraphicEngine::GraphicEngineProps;
  using Target = vk_core::GraphicEngine;
public:
  GraphicApiAdapter(InitParams &&Params){Obj = std::make_unique<Target>(std::move(Params));}
};

#endif // ENGINE_SRC_MODULES_VK_CORE_ADAPTERS_FABRIC_HPP
