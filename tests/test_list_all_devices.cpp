//
// Created by FullHat on 29/03/2024.
//
#include <gtest/gtest.h>
#include <vulkan/vulkan.hpp>

// Test fixture for Vulkan device tests
class VulkanDeviceTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    // Initialize Vulkan instance
    instance = vk::createInstance({});
  }

  static void TearDownTestSuite() {
    // Destroy Vulkan instance
    if (instance) {
      instance.destroy();
    }
  }

  static vk::Instance instance;
};

vk::Instance VulkanDeviceTest::instance;

// Test case to list all Vulkan devices
TEST_F(VulkanDeviceTest, ListDevices) {
  ASSERT_TRUE(instance); // Ensure Vulkan instance is valid

  // Enumerate physical devices
  std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();

  // Check if there is at least one physical device
  ASSERT_FALSE(physicalDevices.empty());

  // Output information about each physical device
  for (const auto& physicalDevice : physicalDevices) {
    vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
    std::cout << "Device Name: " << properties.deviceName << std::endl;
    // Print other properties as needed
  }
}

// Entry point for running tests
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
