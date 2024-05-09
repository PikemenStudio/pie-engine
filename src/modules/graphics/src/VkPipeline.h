//
// Created by FullHat on 06/05/2024.
//

#ifndef ENGINE_SRC_MODULES_GRAPHICS_SRC_VKPIPELINE_H
#define ENGINE_SRC_MODULES_GRAPHICS_SRC_VKPIPELINE_H

#include "../../windows/facades/facade.hpp"
#include "VkPhysicalDevice.hpp"
#include "VkInstance.hpp"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan.hpp"
#include <vector>
#include <map>

namespace vk_core {

template <WindowApiImpl WindowImpl>
class VkPhysicalDevice;

/*
 * Vulkan wrapper. Operates with Pipeline, SwapChain
 * Required to Render anything through Vulkan
 * Here you can set up render settings
 *
 * Require initialized vk components
 *  1) vk::PhysicalDevice
 *  2) vk::Instance
 *
 * Require initialized facades
 *  1) WindowFacade<Impl> (Support Impls: Glfw)
 *
 * Usage:
 *  1) Initialize by constructor(Props) -> no return params, can throw exception
 *
 */
template <WindowApiImpl WindowImpl> class VkPipeline {
public:
  struct FacadesStruct {
    std::shared_ptr<WindowApiFacade<WindowImpl>> Window;
  };

  struct VkPipelineProps;

  VkPipeline(VkPipelineProps Props);
  ~VkPipeline();

  struct VkPipelineProps {
    std::shared_ptr<typename vk_core::VkPhysicalDevice<WindowImpl>> PhysicalDevice;
    std::shared_ptr<vk_core::VkInstance> Instance;
    FacadesStruct Facades;
    std::map<vk::QueueFlagBits, uint32_t> FamilyIndexes;
  };

protected:
  struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR Capabilities;
    std::vector<vk::SurfaceFormatKHR> Formats;
    std::vector<vk::PresentModeKHR> PresentModes;
  };

  struct SwapChainFrameStruct {
    vk::Image Image;
    vk::ImageView ImageView;
  };

  struct SwapChainBundleStruct {
    vk::SwapchainKHR Swapchain;
    std::vector<SwapChainFrameStruct> Frames;
    vk::Format Format;
    vk::Extent2D Extent;
  };
  std::optional<SwapChainBundleStruct> SwapChainBundle;

  struct NativeComponentsStruct {
    std::shared_ptr<typename vk_core::VkPhysicalDevice<WindowImpl>> PhysicalDevice;
    std::shared_ptr<vk_core::VkInstance> Instance;

    std::optional<FacadesStruct> Facades;
    std::map<vk::QueueFlagBits, uint32_t> FamilyIndexes;

    std::optional<vk::SurfaceKHR> Surface;

    std::optional<SwapChainSupportDetails> SwapChainSupport;

  } NativeComponents;

protected:

protected:
  // SwapChain
  void initWindowSurface();
  void querySwapChainSupport();
  vk::SurfaceFormatKHR chooseSwapChainSurfaceFormat();
  vk::PresentModeKHR chooseSwapChainPresentFormat();
  vk::Extent2D chooseSwapChainExtent(std::pair<uint32_t, uint32_t> SizeWH);
  void logSwapChainInfo();
  void logPresentMode(vk::PresentModeKHR Mode);
  void logTransformBits(vk::SurfaceTransformFlagsKHR Transform);
  void logAlphaBits(vk::CompositeAlphaFlagsKHR Alpha);
  void logImageBits(vk::ImageUsageFlags ImageFlags);
  void createSwapChain();
};

} // namespace vk_core

#endif // ENGINE_SRC_MODULES_GRAPHICS_SRC_VKPIPELINE_H
