//
// Created by FullHat on 03/04/2024.
//

#ifndef ENGINE_SRC_MODULES_VK_CORE_SRC_GRAPHICENGINE_HPP
#define ENGINE_SRC_MODULES_VK_CORE_SRC_GRAPHICENGINE_HPP

#include "../../windows/facades/facade.hpp"
#include "VkInstance.hpp"
#include "VkPhysicalDevice.hpp"
#include "VkPipeline.h"
#include "loguru/loguru.hpp"

namespace vk_core {

template <WindowApiImpl WindowImpl> class GraphicEngine {
public:
  struct GraphicEngineProps;

  GraphicEngine(GraphicEngineProps Props);
  ~GraphicEngine();

  struct GraphicEngineProps {
    WindowApiFacade<WindowImpl> Window;

    vk_core::VkInstance::VkInstanceProps VkInstanceProps;
    vk_core::VkPhysicalDevice<WindowImpl>::VkPhysicalDeviceProps VkPhysicalDeviceProps;
  };

  void setupInstance(VkInstance::VkInstanceProps Props);
  void setupPhysicalDevice(VkPhysicalDevice<WindowImpl>::VkPhysicalDeviceProps Props);

  std::vector<typename VkPhysicalDevice<WindowImpl>::PhysicalDeviceLocalProps>
  getLocalPhysicalDevices() const;

  void chooseLocalPhysicalDevice(
      const VkPhysicalDevice<WindowImpl>::PhysicalDeviceLocalProps &Device);

  enum class DeviceChoosePolicy : uint_fast8_t {
    FIRST,
    BEST,
  };
  // Call with default choose policy
  // If ChooseAnyWayIfFailed is true, then the function will try to choose any
  // in the BEST policy ONLY
  void chooseLocalPhysicalDevice(const DeviceChoosePolicy Policy);

private:
  struct AdaptersStruct {
    std::shared_ptr<WindowApiFacade<WindowImpl>> Window;
  };

  struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR Capabilities;
    std::vector<vk::SurfaceFormatKHR> Formats;
    std::vector<vk::PresentModeKHR> PresentModes;
  };

  struct NativeComponentsStruct {
    std::optional<AdaptersStruct> Adapters;

    std::shared_ptr<vk_core::VkInstance> Instance;
    std::shared_ptr<vk_core::VkPhysicalDevice<WindowImpl>> PhysicalDevice;
  } NativeComponents;
};

} // namespace vk_core

#endif // ENGINE_SRC_MODULES_VK_CORE_SRC_GRAPHICENGINE_HPP
