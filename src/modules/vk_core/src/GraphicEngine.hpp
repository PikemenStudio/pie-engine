//
// Created by FullHat on 03/04/2024.
//

#ifndef ENGINE_SRC_MODULES_VK_CORE_SRC_GRAPHICENGINE_HPP
#define ENGINE_SRC_MODULES_VK_CORE_SRC_GRAPHICENGINE_HPP

#include "../../windows/adapters/fabric.hpp"
#include "VkInstance.hpp"
#include "VkPhysicalDevice.hpp"

namespace vk_core {

class GraphicEngine {
public:
  struct GraphicEngineProps;

  GraphicEngine(GraphicEngineProps Props);
  ~GraphicEngine();

  struct GraphicEngineProps {
    WindowAdapter Window;

    vk_core::VkInstance::VkInstanceProps VkInstanceProps;
    vk_core::VkPhysicalDevice::VkPhysicalDeviceProps VkPhysicalDeviceProps;
  };

  void setupInstance(VkInstance::VkInstanceProps Props);
  void setupPhysicalDevice(VkPhysicalDevice::VkPhysicalDeviceProps Props);

private:
  struct AdaptersStruct {
    WindowAdapter Window;
  };

  struct NativeComponentsStruct {
    std::optional<AdaptersStruct> Adapters;

    std::optional<vk_core::VkInstance> Instance;
    std::optional<vk_core::VkPhysicalDevice> PhysicalDevice;
  } NativeComponents;
};

} // namespace vk_core

#endif // ENGINE_SRC_MODULES_VK_CORE_SRC_GRAPHICENGINE_HPP
