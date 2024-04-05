//
// Created by FullHat on 30/03/2024.
//

#ifndef ENGINE_SRC_MODULES_VK_CORE_SRC_VKINSTANCE_HPP
#define ENGINE_SRC_MODULES_VK_CORE_SRC_VKINSTANCE_HPP

#include <string>
#include <vulkan/vulkan.hpp>

namespace vk_core {

class VkInstance {
public:
  struct VkInstanceProps;

  VkInstance(VkInstanceProps Props);
  ~VkInstance();

  VkInstance(const VkInstance &InstanceToCopy) = delete;
  VkInstance(VkInstance &&InstanceToMove);
  VkInstance &operator=(const VkInstance &InstanceToCopy) = delete;
  VkInstance &operator=(VkInstance &&InstanceToMove);

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
    std::vector<const char *> RequestedWindowExtensions;
  };

  operator vk::Instance() { return NativeVkInstance; }

protected:
  bool
  isApiSupportExtensions(const std::vector<const char *> &ExtensionNames) const;
  bool isApiSupportLayers(const std::vector<const char *> &LayerNames) const;

  static void swap(VkInstance &I1, VkInstance &I2);

  struct DebugMessenger {
    DebugMessenger(vk::Instance *Instance);
    ~DebugMessenger();

    DebugMessenger(const DebugMessenger &MessengerToCopy) = delete;
    DebugMessenger(DebugMessenger &&MessengerToMove);
    DebugMessenger &operator=(const DebugMessenger &MessengerToCopy) = delete;
    DebugMessenger &operator=(DebugMessenger &&MessengerToMove) = delete;

    static void swap(std::optional<DebugMessenger> &M1,
                     std::optional<DebugMessenger> &M2);

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(
        [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
        [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT MessageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *PCallbackData,
                  void *PUserData);

    vk::DebugUtilsMessengerEXT Messenger;
    vk::DispatchLoaderDynamic Loader;

    vk::Instance *Instance;
  };
  std::optional<DebugMessenger> DebugMessengerInstance;

protected:
  uint32_t Version;

  std::vector<const char *> Extensions;

  inline static constinit bool Debug = true;

  vk::Instance NativeVkInstance = nullptr;
};

} // namespace vk_core

#endif // ENGINE_SRC_MODULES_VK_CORE_SRC_VKINSTANCE_HPP
