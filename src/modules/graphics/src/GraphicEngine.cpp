//
// Created by FullHat on 03/04/2024.
//

#include "GraphicEngine.hpp"
#include "loguru/loguru.hpp"

// Include only if glfw implementation required
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vk_core {
template <WindowApiImpl WindowImpl>
void GraphicEngine<WindowImpl>::setupInstance(
    VkInstance::VkInstanceProps Props) {
  LOG_F(INFO, "Setting up instance");
  NativeComponents.Instance = std::make_shared<vk_core::VkInstance>(Props);
  LOG_F(INFO, "Instance set up");
}

template <WindowApiImpl WindowImpl>
void GraphicEngine<WindowImpl>::setupPhysicalDevice(
    VkPhysicalDevice::VkPhysicalDeviceProps Props) {
  LOG_F(INFO, "Setting up physical device");
  NativeComponents.PhysicalDevice =
      std::make_shared<vk_core::VkPhysicalDevice>(Props);
  LOG_F(INFO, "Physical device set up");
}

template <WindowApiImpl WindowImpl>
GraphicEngine<WindowImpl>::GraphicEngine(
    GraphicEngine::GraphicEngineProps Props) {
  LOG_F(INFO, "Creating Graphic Engine");

  NativeComponents.Adapters.emplace(
      AdaptersStruct{.Window = std::move(Props.Window)});

  setupInstance(VkInstance::VkInstanceProps(Props.VkInstanceProps));

  if (Props.VkPhysicalDeviceProps.Instance == nullptr) {
    Props.VkPhysicalDeviceProps.Instance = NativeComponents.Instance;
  }
  setupPhysicalDevice(
      VkPhysicalDevice::VkPhysicalDeviceProps(Props.VkPhysicalDeviceProps));
  LOG_F(INFO, "Graphic Engine created");
}

template <WindowApiImpl WindowImpl>
GraphicEngine<WindowImpl>::~GraphicEngine() {
  NativeComponents.PhysicalDevice.reset();

  static_cast<vk::Instance &>(*NativeComponents.Instance)
      .destroySurfaceKHR(NativeComponents.Surface.value());
  NativeComponents.Surface.reset();

  NativeComponents.Instance.reset();
}

template <WindowApiImpl WindowImpl>
std::vector<VkPhysicalDevice::PhysicalDeviceLocalProps>
GraphicEngine<WindowImpl>::getLocalPhysicalDevices() const {
  if (NativeComponents.PhysicalDevice == nullptr) {
    LOG_F(INFO, "Physical device is null");
    throw std::runtime_error("Physical device is null");
  }

  return NativeComponents.PhysicalDevice->getLocalPhysicalDevices();
}

template <WindowApiImpl WindowImpl>
void GraphicEngine<WindowImpl>::chooseLocalPhysicalDevice(
    const VkPhysicalDevice::PhysicalDeviceLocalProps &Device) {
  if (NativeComponents.PhysicalDevice == nullptr) {
    LOG_F(INFO, "Physical device is null");
    throw std::runtime_error("Physical device is null");
  }
  this->NativeComponents.PhysicalDevice->chooseDeviceAndSetup(
      [&](const vk::PhysicalDevice &ArgDevice) -> bool {
        return Device == ArgDevice;
      });

  this->NativeComponents.PhysicalDevice->findQueueIndexesAndSetup();

  this->NativeComponents.PhysicalDevice->setupLogicalDevice();
}

template <WindowApiImpl WindowImpl>
void GraphicEngine<WindowImpl>::chooseLocalPhysicalDevice(
    const DeviceChoosePolicy Policy) {
  LOG_F(INFO, "Choosing local physical device");

  if (NativeComponents.PhysicalDevice == nullptr) {
    LOG_F(INFO, "Physical device is null");
    throw std::runtime_error("Physical device is null");
  }

  switch (Policy) {
  case DeviceChoosePolicy::FIRST:
    this->NativeComponents.PhysicalDevice->chooseDeviceAndSetup(
        [](const vk::PhysicalDevice &ArgDevice) -> bool { return true; });
    break;
  case DeviceChoosePolicy::BEST:
    this->NativeComponents.PhysicalDevice->chooseDeviceAndSetup(
        [](const vk::PhysicalDevice &ArgDevice) -> bool {
          const auto Properties = ArgDevice.getProperties();
          return Properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
        });
    break;
  }

  this->NativeComponents.PhysicalDevice->findQueueIndexesAndSetup();

  this->NativeComponents.PhysicalDevice->setupLogicalDevice();

  this->NativeComponents.PhysicalDevice->setupQueues();

  initWindowSurface();

  LOG_F(INFO, "Local physical device chosen");
}

template <>
void GraphicEngine<
    window_api_impls::WindowApiFacadeGlfwImpl>::initWindowSurface() {
  auto *NativeWindow =
      this->NativeComponents.Adapters->Window.ImplInstance.getNativeType();

  VkSurfaceKHR CStyleSurface;
  const auto Result = glfwCreateWindowSurface(
      static_cast<vk::Instance>(*this->NativeComponents.Instance),
      (GLFWwindow *)NativeWindow, nullptr, &CStyleSurface);

  if (Result != VK_SUCCESS) {
    LOG_F(ERROR, "Failed to create window surface");
    throw std::runtime_error("Failed to create window surface");
  }

  this->NativeComponents.Surface = CStyleSurface;
}

template <WindowApiImpl WindowImpl>
void GraphicEngine<WindowImpl>::querySwapChainSupport() {
  if (NativeComponents.PhysicalDevice == nullptr || !NativeComponents.Surface) {
    LOG_F(ERROR, "Physical device or surface is not initialized");
    throw std::runtime_error("Physical device or surface is not initialized");
  }

  const auto Device =
      static_cast<vk::PhysicalDevice>(*NativeComponents.PhysicalDevice);
  const auto Surface = NativeComponents.Surface.value();

  NativeComponents.SwapChainSupport = SwapChainSupportDetails{
      .Capabilities = Device.getSurfaceCapabilitiesKHR(Surface),
      .Formats = Device.getSurfaceFormatsKHR(Surface),
      .PresentModes = Device.getSurfacePresentModesKHR(Surface),
  };
}

template class vk_core::GraphicEngine<
    window_api_impls::WindowApiFacadeGlfwImpl>;

} // namespace vk_core