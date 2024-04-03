//
// Created by FullHat on 02/04/2024.
//

#ifndef ENGINE_SRC_MODULES_VK_CORE_ADAPTERS_FABRIC_HPP
#define ENGINE_SRC_MODULES_VK_CORE_ADAPTERS_FABRIC_HPP

#include "../../../adapters_interfaces/fabric_adapter.hpp"
#include "../src/VkInstance.hpp"
#include "../src/VkPhysicalDEvice.hpp"

template <typename T>
concept GraphicApiAdapterMethods = requires(T t) {
  { t.test() } -> std::same_as<void>;
};

class GraphicApiAdapter : public Adapter<vk_core::VkInstance, vk_core::VkInstance::VkInstanceProps> {
  using InitParams = vk_core::VkInstance::VkInstanceProps;
  using Target = vk_core::VkInstance;
public:
  GraphicApiAdapter(InitParams Params){Obj = std::make_unique<Target>(Params);}
};

class GraphicApiAdapter1 : public Adapter<vk_core::VkPhysicalDevice, vk_core::VkPhysicalDevice> {
  using InitParams = vk_core::VkPhysicalDevice::VkPhysicalDeviceProps;
  using Target = vk_core::VkPhysicalDevice;
public:
  GraphicApiAdapter1(InitParams Params){Obj = std::make_unique<Target>(Params);}
};

#endif // ENGINE_SRC_MODULES_VK_CORE_ADAPTERS_FABRIC_HPP
