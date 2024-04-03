//
// Created by FullHat on 03/04/2024.
//

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VkPhysicalDevice.hpp"
#include "loguru/loguru.hpp"

namespace vk_core {
VkPhysicalDevice::VkPhysicalDevice(
    VkPhysicalDevice::VkPhysicalDeviceProps Props) {
  Instance = Props.Instance;
}
std::vector<vk::PhysicalDevice>
VkPhysicalDevice::getNativePhysicalDevices() const {
  if (Instance == nullptr) {
    LOG_F(ERROR, "Instance is not initialized");
    throw std::runtime_error("Instance is not initialized");
  }

  const auto Devices =
      static_cast<vk::Instance>(*Instance).enumeratePhysicalDevices();

  std::for_each(Devices.begin(), Devices.end(), [&](auto &Device) {
    const auto Properties = Device.getProperties();
    LOG_F(INFO, "Physical device properties:");
    // Print all properties of device
    LOG_F(INFO, "Physical device properties:");
    LOG_F(INFO, "Device Name: %s", std::string(Properties.deviceName).c_str());
    LOG_F(INFO, "Device Type: %d", static_cast<int>(Properties.deviceType));
    LOG_F(INFO, "API Version: %d.%d.%d",
          VK_VERSION_MAJOR(Properties.apiVersion),
          VK_VERSION_MINOR(Properties.apiVersion),
          VK_VERSION_PATCH(Properties.apiVersion));
    LOG_F(INFO, "Driver Version: %d.%d.%d",
          VK_VERSION_MAJOR(Properties.driverVersion),
          VK_VERSION_MINOR(Properties.driverVersion),
          VK_VERSION_PATCH(Properties.driverVersion));
    LOG_F(INFO, "Vendor ID: %d", Properties.vendorID);
    LOG_F(INFO, "Device ID: %d", Properties.deviceID);
  });
  return Devices;
}
} // namespace vk_core