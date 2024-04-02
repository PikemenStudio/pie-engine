//
// Created by FullHat on 30/03/2024.
//

#ifndef ENGINE_SRC_MODULES_VK_CORE_SRC_VKINSTANCE_H
#define ENGINE_SRC_MODULES_VK_CORE_SRC_VKINSTANCE_H

#include <string>
#include <vulkan/vulkan.hpp>


namespace vk_core {

class VkInstance {
public:
  struct VkInstanceProps;

  VkInstance(VkInstanceProps Props);

  struct VkInstanceProps {
    struct Version {
      uint32_t Major;
      uint32_t Minor;
      uint32_t Patch;

      uint32_t toVkVersion() { return VK_MAKE_VERSION(Major, Minor, Patch); }
    };

    std::string AppName;
    std::string EngineName;
    Version AppVersion;
    Version EngineVersion;

    // Window dependency
    std::vector<char *> RequestedWindowExtensions;
  };

protected:
  uint32_t Version;
};

} // namespace vk_core

#endif // ENGINE_SRC_MODULES_VK_CORE_SRC_VKINSTANCE_H
