//
// Created by FullHat on 03/04/2024.
//

#include "GraphicEngine.hpp"
#include "loguru/loguru.hpp"

namespace vk_core {
void GraphicEngine::setupInstance(VkInstance::VkInstanceProps Props) {
  LOG_F(INFO, "Setting up instance");
  auto &&Core = vk_core::VkInstance(Props);
  NativeComponents.Instance = std::move(Core);
  LOG_F(INFO, "Instance set up");
}
void GraphicEngine::setupPhysicalDevice(
    VkPhysicalDevice::VkPhysicalDeviceProps Props) {
  LOG_F(INFO, "Setting up physical device");
  NativeComponents.PhysicalDevice = vk_core::VkPhysicalDevice(Props);
  LOG_F(INFO, "Physical device set up");
}
GraphicEngine::GraphicEngine(GraphicEngine::GraphicEngineProps Props) {
  LOG_F(INFO, "Creating Graphic Engine");
  setupInstance(VkInstance::VkInstanceProps(Props.VkInstanceProps));
  setupPhysicalDevice(
      VkPhysicalDevice::VkPhysicalDeviceProps(Props.VkPhysicalDeviceProps));
  LOG_F(INFO, "Graphic Engine created");
}
GraphicEngine::~GraphicEngine() {
  NativeComponents.PhysicalDevice.reset();

  NativeComponents.Instance.reset();
}
} // namespace vk_core