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
    DebugMessengerInstance.emplace(&NativeVkInstance);
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
  LOG_F(INFO, "Destroying Vulkan instance");
  if (DebugMessengerInstance.has_value()) {
    DebugMessengerInstance.reset();
  }

  if (NativeVkInstance) {
    NativeVkInstance.destroy();
    NativeVkInstance = nullptr;
    LOG_F(INFO, "Vulkan instance destroyed");
  }
}

VkInstance &VkInstance::operator=(VkInstance &&InstanceToMove) {
  swap(InstanceToMove, *this);
  return *this;
}

void VkInstance::swap(VkInstance &I1, VkInstance &I2) {
  VkInstance::DebugMessenger::swap(I1.DebugMessengerInstance,
                                   I2.DebugMessengerInstance);

  std::swap(I1.NativeVkInstance, I2.NativeVkInstance);
  std::swap(I1.Version, I2.Version);
  std::swap(I1.Extensions, I2.Extensions);
  std::swap(I1.Debug, I2.Debug);

  if (I1.DebugMessengerInstance.has_value()) {
    I1.DebugMessengerInstance.value().Instance = &I1.NativeVkInstance;
  }
  if (I2.DebugMessengerInstance.has_value()) {
    I2.DebugMessengerInstance.value().Instance = &I2.NativeVkInstance;
  }
}

VkInstance::VkInstance(VkInstance &&InstanceToMove) {
  swap(InstanceToMove, *this);
}

VkBool32 VkInstance::DebugMessenger::debugCallback(
    [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT MessageType,
    const VkDebugUtilsMessengerCallbackDataEXT *PCallbackData,
    [[maybe_unused]] void *PUserData) {
  LOG_F(WARNING, "Validation layer: %s", PCallbackData->pMessage);
  return VK_FALSE;
}

VkInstance::DebugMessenger::DebugMessenger(vk::Instance *Instance)
    : Instance(Instance) {
  Loader = vk::DispatchLoaderDynamic(*Instance, vkGetInstanceProcAddr);

  vk::DebugUtilsMessengerCreateInfoEXT CreateInfo =
      vk::DebugUtilsMessengerCreateInfoEXT{
          .flags = vk::DebugUtilsMessengerCreateFlagsEXT(),
          .messageSeverity =
              vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
              vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
              vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
          .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                         vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                         vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
          .pfnUserCallback = DebugMessenger::debugCallback,
          .pUserData = nullptr};

  Messenger =
      Instance->createDebugUtilsMessengerEXT(CreateInfo, nullptr, Loader);
  LOG_F(INFO, "Debug messenger created");
}

VkInstance::DebugMessenger::~DebugMessenger() {
  LOG_F(INFO, "Destroying debug messenger");
  if (Instance == nullptr) {
    LOG_F(INFO, "Instance is null, maybe it is just empty object");
    return;
  }
  Instance->destroyDebugUtilsMessengerEXT(Messenger, nullptr, Loader);
}

VkInstance::DebugMessenger::DebugMessenger(DebugMessenger &&MessengerToMove)
    : Instance(MessengerToMove.Instance) {
  Messenger = std::move(MessengerToMove.Messenger);
  Loader = std::move(MessengerToMove.Loader);
}

void vk_core::VkInstance::DebugMessenger::swap(
    std::optional<DebugMessenger> &M1, std::optional<DebugMessenger> &M2) {
  if (M1.has_value() && M2.has_value()) {
    std::swap(M1.value().Messenger, M2.value().Messenger);
    std::swap(M1.value().Loader, M2.value().Loader);
    // std::swap(M1.value().Instance, M2.value().Instance);
  } else if (M1.has_value()) {
    M2.emplace(std::move(M1.value()));
    M1.value().Instance = nullptr;
    M1.reset();
  } else if (M2.has_value()) {
    M1.emplace(std::move(M2.value()));
    M1.value().Instance = nullptr;
    M2.reset();
  }
}
} // namespace vk_core
  // namespace vk_core