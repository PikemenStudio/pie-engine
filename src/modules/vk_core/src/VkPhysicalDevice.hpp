//
// Created by FullHat on 03/04/2024.
//

#ifndef ENGINE_SRC_MODULES_VK_CORE_SRC_VKPHISICALDEVICE_HPP
#define ENGINE_SRC_MODULES_VK_CORE_SRC_VKPHISICALDEVICE_HPP

#include "VkInstance.hpp"
#include "vulkan/vulkan.hpp"

namespace vk_core {

class VkPhysicalDevice {
public:
  struct VkPhysicalDeviceProps;

  VkPhysicalDevice(VkPhysicalDeviceProps Props);
  ~VkPhysicalDevice();

  struct VkPhysicalDeviceProps {
    std::shared_ptr<VkInstance> Instance;
  };

  std::vector<vk::PhysicalDevice> getNativePhysicalDevices() const;

protected:
  std::shared_ptr<VkInstance> Instance;
};

} // namespace vk_core

#endif // ENGINE_SRC_MODULES_VK_CORE_SRC_VKPHISICALDEVICE_HPP
