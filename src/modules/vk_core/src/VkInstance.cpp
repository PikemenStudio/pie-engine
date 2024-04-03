//
// Created by FullHat on 30/03/2024.
//

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VkInstance.hpp"
#include "loguru/loguru.hpp"
#include <iomanip>

namespace vk_core {
VkInstance::VkInstance(VkInstance::VkInstanceProps Props) {
  vkEnumerateInstanceVersion(&Version);

  vk::ApplicationInfo AppInf{.pApplicationName = Props.AppName.c_str(),
                             .applicationVersion =
                                 Props.AppVersion.toVkVersion(),
                             .pEngineName = Props.EngineName.c_str(),
                             .engineVersion = Props.EngineVersion.toVkVersion(),
                             .apiVersion = Version};

  Extensions = Props.RequestedWindowExtensions;
  if (Version >= VK_MAKE_API_VERSION(0, 1, 3, 216)) {
    Extensions.push_back("VK_KHR_portability_enumeration");
  }
  if (this->Debug) {
    Extensions.push_back("VK_EXT_debug_utils");
  }

  LOG_F(INFO, "Extensions to be requested:");
  std::for_each(Extensions.begin(), Extensions.end(), [&](auto &ExtensionName) {
    LOG_F(INFO, "\"%s\"", ExtensionName);
  });

  std::vector<const char *> Layers;
  if (this->Debug) {
    Layers.push_back("VK_LAYER_KHRONOS_validation");
  }

  if (!isApiSupportExtensions(Extensions) || !isApiSupportLayers(Layers)) {
    throw std::runtime_error("Vulkan instance creation failed");
  }

  vk::InstanceCreateInfo CreateInf = vk::InstanceCreateInfo{
      .flags = vk::InstanceCreateFlags(),
      .pApplicationInfo = &AppInf,
      .enabledLayerCount = static_cast<uint32_t>(Layers.size()),
      .ppEnabledLayerNames = Layers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(Extensions.size()),
      .ppEnabledExtensionNames = Extensions.data(),
  };

  if (Version >= VK_MAKE_API_VERSION(0, 1, 3, 216)) {
    CreateInf.flags = vk::InstanceCreateFlags(
        VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR);
  }

  try {
    NativeVkInstance = vk::createInstance(CreateInf);
    LOG_F(INFO, "Vulkan instance created successfully");
  } catch (vk::SystemError &Err) {
    LOG_F(ERROR, "Vulkan instance creation failed: %s", Err.what());
    throw;
  }

  if (Debug) {
    DebugMessengerInstance.emplace(NativeVkInstance);
  }
}
bool VkInstance::isApiSupportExtensions(
    const std::vector<const char *> &ExtensionNames) const {
  std::vector<vk::ExtensionProperties> SupportedExtensions =
      vk::enumerateInstanceExtensionProperties();

  for (const auto &RequiredExtensionName : ExtensionNames) {
    auto It =
        std::find_if(SupportedExtensions.begin(), SupportedExtensions.end(),
                     [&](auto &SupportedExtension) {
                       return strcmp(SupportedExtension.extensionName,
                                     RequiredExtensionName) == 0;
                     });
    if (It == SupportedExtensions.end()) {
      LOG_F(ERROR, "Extension \"%s\" is not supported", RequiredExtensionName);
      return false;
    }
  }

  return true;
}
bool VkInstance::isApiSupportLayers(
    const std::vector<const char *> &LayerNames) const {
  std::vector<vk::LayerProperties> SupportedLayers =
      vk::enumerateInstanceLayerProperties();

  for (const auto &RequiredLayerName : LayerNames) {
    auto It = std::find_if(SupportedLayers.begin(), SupportedLayers.end(),
                           [&](auto &SupportedLayer) {
                             return strcmp(SupportedLayer.layerName,
                                           RequiredLayerName) == 0;
                           });
    if (It == SupportedLayers.end()) {
      LOG_F(ERROR, "Layer \"%s\" is not supported", RequiredLayerName);
      return false;
    }
  }

  return true;
}
VkInstance::~VkInstance() {
  if (DebugMessengerInstance.has_value()) {
    DebugMessengerInstance.reset();
  }

  if (NativeVkInstance) {
    NativeVkInstance.destroy();
    NativeVkInstance = nullptr;
    LOG_F(INFO, "Vulkan instance destroyed");
  }
}
VkBool32 VkInstance::DebugMessenger::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT MessageType,
    const VkDebugUtilsMessengerCallbackDataEXT *PCallbackData,
    void *PUserData) {
    LOG_F(WARNING, "Validation layer: %s", PCallbackData->pMessage);
    return VK_FALSE;
}
VkInstance::DebugMessenger::DebugMessenger(vk::Instance &Instance)
    : Instance(Instance) {
  Loader = vk::DispatchLoaderDynamic(Instance, vkGetInstanceProcAddr);

  vk::DebugUtilsMessengerCreateInfoEXT CreateInfo = vk::DebugUtilsMessengerCreateInfoEXT{
      .flags = vk::DebugUtilsMessengerCreateFlagsEXT(),
      .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
      .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
      .pfnUserCallback = DebugMessenger::debugCallback,
      .pUserData = nullptr
  };

  Messenger = Instance.createDebugUtilsMessengerEXT(CreateInfo, nullptr, Loader);
}
VkInstance::DebugMessenger::~DebugMessenger() {
  if (Instance == nullptr) {
    LOG_F(ERROR, "Instance destroyed before DebugMessenger");
    return;
  }
  Instance.destroyDebugUtilsMessengerEXT(Messenger, nullptr, Loader);
}
} // namespace vk_core