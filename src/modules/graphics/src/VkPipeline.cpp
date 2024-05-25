//
// Created by FullHat on 06/05/2024.
//

#include "VulkanPipeline.h"

#include "loguru/loguru.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <set>

using namespace vk_core;

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    VulkanPipeline(VulkanPipeline::VkPipelineProps Props) {
  this->NativeComponents.PhysicalDevice = Props.PhysicalDevice;
  this->NativeComponents.Instance = Props.Instance;
  this->NativeComponents.Facades = {.Window = Props.Facades.Window,
                                    .ShaderLoader = Props.Facades.ShaderLoader,
                                    .SceneManager = Props.Facades.SceneManager};
  this->NativeComponents.FamilyIndexes = std::move(Props.FamilyIndexes);
  this->NativeComponents.GraphicsQueue = Props.GraphicsQueue;
  this->NativeComponents.PresentQueue = Props.PresentQueue;

  initWindowSurface();
  createSwapChain();
  createDescriptorSetLayouts({.Count = 2,
                              .Indexes = {0, 1},
                              .Types = {vk::DescriptorType::eUniformBuffer,
                                        vk::DescriptorType::eStorageBuffer},
                              .Counts = {1, 1},
                              .Stages = {vk::ShaderStageFlagBits::eVertex,
                                         vk::ShaderStageFlagBits::eVertex}});
  createPipeline(
      {.VertexShaderPath = "/Users/fullhat/Documents/GitHub/pie-engine/tests/"
                           "resources/test.vert",
       .FragmentShaderPath = "/Users/fullhat/Documents/GitHub/pie-engine/tests/"
                             "resources/test.frag"});
  createFrameBuffers();
  createCommandPool();
  createFrameCommandBuffers();
  createMainCommandBuffer();
  createSemaphores();
  createFences();
  LOG_F(INFO, "Init pipeline, GOOD");
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                        SceneManagerImplT>::~VulkanPipeline() {
  if (this->NativeComponents.PhysicalDevice == nullptr) {
    LOG_F(ERROR, "Device is null");
  }

  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
      .destroyCommandPool(this->CommandPool);

  if (PipelineBundle.has_value()) {
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .destroyPipeline(PipelineBundle.value().Pipeline);
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .destroyPipelineLayout(PipelineBundle.value().Layout);
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .destroyRenderPass(PipelineBundle.value().RenderPass);
  }

  destroySwapChain();

  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
      .destroyDescriptorSetLayout(this->FrameSetLayout);

  static_cast<vk::Instance &>(*NativeComponents.Instance)
      .destroySurfaceKHR(NativeComponents.Surface.value());
  NativeComponents.Surface.reset();
}

template <>
void vk_core::VulkanPipeline<
    window_api_impls::WindowApiFacadeGlfwImpl,
    shader_loader_impls::ShaderLoaderSimpleImpl,
    scene_manager_facades::SceneManagerBaseImpl<
        scene_manager_facades::SceneManagerDependencies>>::initWindowSurface() {
  auto *NativeWindow =
      this->NativeComponents.Facades->Window->ImplInstance.getNativeType();

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

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                             SceneManagerImplT>::querySwapChainSupport() {
  if (NativeComponents.PhysicalDevice == nullptr || !NativeComponents.Surface) {
    LOG_F(ERROR, "Physical device or surface is not initialized");
    throw std::runtime_error("Physical device or surface is not initialized");
  }

  const auto Surface = NativeComponents.Surface.value();

  NativeComponents.SwapChainSupport = SwapChainSupportDetails{
      .Capabilities =
          static_cast<vk::PhysicalDevice>(*NativeComponents.PhysicalDevice)
              .getSurfaceCapabilitiesKHR(Surface),
      .Formats =
          static_cast<vk::PhysicalDevice>(*NativeComponents.PhysicalDevice)
              .getSurfaceFormatsKHR(Surface),
      .PresentModes =
          static_cast<vk::PhysicalDevice>(*NativeComponents.PhysicalDevice)
              .getSurfacePresentModesKHR(Surface),
  };

  logSwapChainInfo();
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
vk::SurfaceFormatKHR
vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                        SceneManagerImplT>::chooseSwapChainSurfaceFormat() {
  for (vk::SurfaceFormatKHR Format :
       this->NativeComponents.SwapChainSupport.value().Formats) {
    if (Format.format == vk::Format::eB8G8R8A8Unorm &&
        Format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
      return Format;
    }
  }

  return this->NativeComponents.SwapChainSupport.value().Formats[0];
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
vk::PresentModeKHR
vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                        SceneManagerImplT>::chooseSwapChainPresentFormat() {
  for (vk::PresentModeKHR PresentMode :
       this->NativeComponents.SwapChainSupport.value().PresentModes) {
    if (PresentMode == vk::PresentModeKHR::eMailbox) {
      return PresentMode;
    }
  }

  return vk::PresentModeKHR::eFifo;
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
vk::Extent2D
vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    chooseSwapChainExtent(std::pair<uint32_t, uint32_t> SizeWH) {
  const auto Capabilities =
      this->NativeComponents.SwapChainSupport.value().Capabilities;

  if (Capabilities.currentExtent.width != UINT32_MAX) {
    return Capabilities.currentExtent;
  }

  vk::Extent2D Extent = {SizeWH.first, SizeWH.second};

  Extent.width =
      std::min(Capabilities.maxImageExtent.width,
               std::max(Capabilities.minImageExtent.width, SizeWH.first));

  Extent.height =
      std::min(Capabilities.maxImageExtent.height,
               std::max(Capabilities.minImageExtent.height, SizeWH.second));

  return Extent;
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                             SceneManagerImplT>::logSwapChainInfo() {
  auto &Support = this->NativeComponents.SwapChainSupport.value();
  LOG_F(INFO, "Swapchain can support the following surface capabilities:\n");

  LOG_F(INFO, "\tminimum image count: %d\n",
        Support.Capabilities.minImageCount);
  LOG_F(INFO, "\tmaximum image count: %d\n",
        Support.Capabilities.maxImageCount);

  LOG_F(INFO, "\tcurrent extent: \n");

  LOG_F(INFO, "\t\twidth: %d", Support.Capabilities.currentExtent.width);
  LOG_F(INFO, "\t\theight: %d", Support.Capabilities.currentExtent.height);

  LOG_F(INFO, "\tminimum supported extent:");
  LOG_F(INFO, "\t\twidth: %d", Support.Capabilities.minImageExtent.width);
  LOG_F(INFO, "\t\theight: %d", Support.Capabilities.minImageExtent.height);

  LOG_F(INFO, "\tmaximum supported extent:");
  LOG_F(INFO, "\t\twidth: %d", Support.Capabilities.maxImageExtent.width);
  LOG_F(INFO, "\t\theight: %d", Support.Capabilities.maxImageExtent.height);
  LOG_F(INFO, "\tmaximum image array layers: %d",
        Support.Capabilities.maxImageArrayLayers);

  LOG_F(INFO, "\tsupported transforms:");
  logTransformBits(Support.Capabilities.supportedTransforms);

  LOG_F(INFO, "\tcurrent transform:\n");
  logTransformBits(Support.Capabilities.currentTransform);

  LOG_F(INFO, "\tsupported alpha operations:\n");
  logAlphaBits(Support.Capabilities.supportedCompositeAlpha);

  LOG_F(INFO, "\tsupported image usage:\n");
  logImageBits(Support.Capabilities.supportedUsageFlags);

  for (vk::SurfaceFormatKHR supportedFormat : Support.Formats) {
    LOG_F(INFO, "supported pixel format: %s",
          vk::to_string(supportedFormat.format).c_str());
    LOG_F(INFO, "supported color space: %s",
          vk::to_string(supportedFormat.colorSpace).c_str());
  }

  for (vk::PresentModeKHR presentMode : Support.PresentModes) {
    logPresentMode(presentMode);
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    logPresentMode(vk::PresentModeKHR Mode) {
  switch (Mode) {
  case vk::PresentModeKHR::eImmediate:
    LOG_F(
        INFO,
        "immediate: the presentation engine does not wait for a vertical blanking period \
to update the current image, meaning this mode may result in visible tearing. No internal \
queuing of presentation requests is needed, as the requests are applied immediately.");
    break;
  case vk::PresentModeKHR::eMailbox:
    LOG_F(
        INFO,
        "mailbox: the presentation engine waits for the next vertical blanking period \
to update the current image. Tearing cannot be observed. An internal single-entry queue is \
used to hold pending presentation requests. If the queue is full when a new presentation \
request is received, the new request replaces the existing entry, and any images associated \
with the prior entry become available for re-use by the application. One request is removed \
from the queue and processed during each vertical blanking period in which the queue is non-empty.");
    break;
  case vk::PresentModeKHR::eFifo:
    LOG_F(INFO,
          "fifo: the presentation engine waits for the next vertical blanking \
period to update the current image. Tearing cannot be observed. An internal queue is used to \
hold pending presentation requests. New requests are appended to the end of the queue, and one \
request is removed from the beginning of the queue and processed during each vertical blanking \
period in which the queue is non-empty. This is the only value of presentMode that is required \
to be supported.");
    break;
  case vk::PresentModeKHR::eFifoRelaxed:
    LOG_F(
        INFO,
        "relaxed fifo: the presentation engine generally waits for the next vertical \
blanking period to update the current image. If a vertical blanking period has already passed \
since the last update of the current image then the presentation engine does not wait for \
another vertical blanking period for the update, meaning this mode may result in visible tearing \
in this case. This mode is useful for reducing visual stutter with an application that will \
mostly present a new image before the next vertical blanking period, but may occasionally be \
late, and present a new image just after the next vertical blanking period. An internal queue \
is used to hold pending presentation requests. New requests are appended to the end of the queue, \
and one request is removed from the beginning of the queue and processed during or after each \
vertical blanking period in which the queue is non-empty.");
    break;
  case vk::PresentModeKHR::eSharedDemandRefresh:
    LOG_F(INFO,
          "shared demand refresh: the presentation engine and application have \
concurrent access to a single image, which is referred to as a shared presentable image. \
The presentation engine is only required to update the current image after a new presentation \
request is received. Therefore the application must make a presentation request whenever an \
update is required. However, the presentation engine may update the current image at any point, \
meaning this mode may result in visible tearing.");
    break;
  case vk::PresentModeKHR::eSharedContinuousRefresh:
    LOG_F(
        INFO,
        "shared continuous refresh: the presentation engine and application have \
concurrent access to a single image, which is referred to as a shared presentable image. The \
presentation engine periodically updates the current image on its regular refresh cycle. The \
application is only required to make one initial presentation request, after which the \
presentation engine must update the current image without any need for further presentation \
requests. The application can indicate the image contents have been updated by making a \
presentation request, but this does not guarantee the timing of when it will be updated. \
This mode may result in visible tearing if rendering to the image is not timed correctly.");
    break;
  default:
    LOG_F(WARNING, "none/undefined");
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    logTransformBits(vk::SurfaceTransformFlagsKHR Transform) {
  if (Transform & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
    LOG_F(INFO, "identity");
  }
  if (Transform & vk::SurfaceTransformFlagBitsKHR::eRotate90) {
    LOG_F(INFO, "90 degree rotation");
  }
  if (Transform & vk::SurfaceTransformFlagBitsKHR::eRotate180) {
    LOG_F(INFO, "180 degree rotation");
  }
  if (Transform & vk::SurfaceTransformFlagBitsKHR::eRotate270) {
    LOG_F(INFO, "270 degree rotation");
  }
  if (Transform & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror) {
    LOG_F(INFO, "horizontal mirror");
  }
  if (Transform & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90) {
    LOG_F(INFO, "horizontal mirror, then 90 degree rotation");
  }
  if (Transform & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180) {
    LOG_F(INFO, "horizontal mirror, then 180 degree rotation");
  }
  if (Transform & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270) {
    LOG_F(INFO, "horizontal mirror, then 270 degree rotation");
  }
  if (Transform & vk::SurfaceTransformFlagBitsKHR::eInherit) {
    LOG_F(INFO, "inherited");
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    logAlphaBits(vk::CompositeAlphaFlagsKHR Alpha) {
  if (Alpha & vk::CompositeAlphaFlagBitsKHR::eOpaque) {
    LOG_F(INFO, "opaque (alpha ignored)");
  }
  if (Alpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) {
    LOG_F(INFO,
          "pre multiplied (alpha expected to already be multiplied in image)");
  }
  if (Alpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) {
    LOG_F(INFO, "post multiplied (alpha will be applied during composition)");
  }
  if (Alpha & vk::CompositeAlphaFlagBitsKHR::eInherit) {
    LOG_F(INFO, "inherited");
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    logImageBits(vk::ImageUsageFlags ImageFlags) {
  if (ImageFlags & vk::ImageUsageFlagBits::eTransferSrc) {
    LOG_F(
        INFO,
        "transfer src: image can be used as the source of a transfer command.");
  }
  if (ImageFlags & vk::ImageUsageFlagBits::eTransferDst) {
    LOG_F(INFO, "transfer dst: image can be used as the destination of a "
                "transfer command.");
  }
  if (ImageFlags & vk::ImageUsageFlagBits::eSampled) {
    LOG_F(
        INFO,
        "sampled: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot either of type VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE or \
VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, and be sampled by a shader.");
  }
  if (ImageFlags & vk::ImageUsageFlagBits::eStorage) {
    LOG_F(
        INFO,
        "storage: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_STORAGE_IMAGE.");
  }
  if (ImageFlags & vk::ImageUsageFlagBits::eColorAttachment) {
    LOG_F(
        INFO,
        "color attachment: image can be used to create a VkImageView suitable for use as \
a color or resolve attachment in a VkFramebuffer.");
  }
  if (ImageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment) {
    LOG_F(INFO,
          "depth/stencil attachment: image can be used to create a VkImageView \
suitable for use as a depth/stencil or depth/stencil resolve attachment in a VkFramebuffer.");
  }
  if (ImageFlags & vk::ImageUsageFlagBits::eTransientAttachment) {
    LOG_F(
        INFO,
        "transient attachment: implementations may support using memory allocations \
with the VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT to back an image with this usage. This \
bit can be set for any image that can be used to create a VkImageView suitable for use as \
a color, resolve, depth/stencil, or input attachment.");
  }
  if (ImageFlags & vk::ImageUsageFlagBits::eInputAttachment) {
    LOG_F(
        INFO,
        "input attachment: image can be used to create a VkImageView suitable for \
occupying VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; be read from \
a shader as an input attachment; and be used as an input attachment in a framebuffer.");
  }
  if (ImageFlags & vk::ImageUsageFlagBits::eFragmentDensityMapEXT) {
    LOG_F(
        INFO,
        "fragment density map: image can be used to create a VkImageView suitable \
for use as a fragment density map image.");
  }
  if (ImageFlags & vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR) {
    LOG_F(
        INFO,
        "fragment shading rate attachment: image can be used to create a VkImageView \
suitable for use as a fragment shading rate attachment or shading rate image");
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                             SceneManagerImplT>::createSwapChain() {
  querySwapChainSupport();
  auto Format = chooseSwapChainSurfaceFormat();
  auto PresentMode = chooseSwapChainPresentFormat();
  auto Extent = chooseSwapChainExtent(
      this->NativeComponents.Facades->Window->ImplInstance.getSize());

  uint32_t ImageCount = std::min(this->NativeComponents.SwapChainSupport.value()
                                     .Capabilities.maxImageCount,
                                 this->NativeComponents.SwapChainSupport.value()
                                         .Capabilities.minImageCount +
                                     1);

  vk::SwapchainCreateInfoKHR CreateInfo{
      .flags = vk::SwapchainCreateFlagsKHR(),
      .surface = this->NativeComponents.Surface.value(),
      .minImageCount = ImageCount,
      .imageFormat = Format.format,
      .imageColorSpace = Format.colorSpace,
      .imageExtent = Extent,
      .imageArrayLayers = 1,
      .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
  };

  std::set<uint32_t> UniqueIndexes;
  for (auto Index : this->NativeComponents.FamilyIndexes) {
    UniqueIndexes.insert(Index.second);
  }
  std::vector<uint32_t> UniqueIndexesVec(UniqueIndexes.begin(),
                                         UniqueIndexes.end());

  if (UniqueIndexesVec.size() > 1) {
    CreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
    CreateInfo.queueFamilyIndexCount = UniqueIndexesVec.size();
    CreateInfo.pQueueFamilyIndices = UniqueIndexesVec.data();
  } else {
    CreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
  }

  CreateInfo.preTransform = this->NativeComponents.SwapChainSupport.value()
                                .Capabilities.currentTransform;
  CreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  CreateInfo.presentMode = PresentMode;
  CreateInfo.clipped = VK_TRUE;

  CreateInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

  SwapChainBundleStruct Bundle;
  try {
    Bundle.Swapchain =
        static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
            .createSwapchainKHR(CreateInfo);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to create swap chain ") +
                             E.what());
  }

  auto Images =
      static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
          .getSwapchainImagesKHR(Bundle.Swapchain);
  Bundle.Frames.reserve(Images.size());
  for (size_t I = 0; I < Images.size(); ++I) {
    vk::ImageViewCreateInfo ViewCreateInfo{
        .image = Images[I],
        .viewType = vk::ImageViewType::e2D,
        .format = Format.format,
        .components =
            {
                .r = vk::ComponentSwizzle::eIdentity,
                .g = vk::ComponentSwizzle::eIdentity,
                .b = vk::ComponentSwizzle::eIdentity,
                .a = vk::ComponentSwizzle::eIdentity,
            },
        .subresourceRange =
            {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    try {
      Bundle.Frames.push_back(SwapChainFrameStruct{
          .Image = Images[I],
          .ImageView =
              static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
                  .createImageView(ViewCreateInfo),
      });
    } catch (vk::SystemError &E) {
      throw std::runtime_error(std::string("Failed to create image view ") +
                               E.what());
    }
  }
  this->SwapChainBundle = Bundle;
  this->SwapChainBundle->Format = Format.format;
  this->SwapChainBundle->Extent = Extent;

  for (auto &Frame : this->SwapChainBundle->Frames) {
    CreateDepthResources(Frame);
  }
  this->SwapChainBundle->DepthFormat =
      this->SwapChainBundle->Frames[0].DepthFormat;

  this->MaxFrameInFlight =
      static_cast<int>(this->SwapChainBundle->Frames.size());
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                             SceneManagerImplT>::destroySwapChain() {
  for (auto &Frame : SwapChainBundle.value().Frames) {
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .destroyImageView(Frame.ImageView);
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .destroyFramebuffer(Frame.FrameBuffer);
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .destroySemaphore(Frame.ImageAvailableSemaphore);
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .destroySemaphore(Frame.RenderFinishedSemaphore);
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .destroyFence(Frame.InFlightFence);

    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .unmapMemory(Frame.CameraMemory);
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .freeMemory(Frame.CameraMemory);
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .destroyBuffer(Frame.CameraBuffer);

    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .unmapMemory(Frame.ModelMemory);
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .freeMemory(Frame.ModelMemory);
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .destroyBuffer(Frame.ModelBuffer);

    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .destroyImage(Frame.DepthBuffer);
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .freeMemory(Frame.DepthBufferMemory);
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .destroyImageView(Frame.DepthBufferView);
  }

  static_cast<vk::Device &>(*NativeComponents.PhysicalDevice)
      .destroySwapchainKHR(SwapChainBundle.value().Swapchain);

  static_cast<vk::Device &>(*NativeComponents.PhysicalDevice)
      .destroyDescriptorPool(FramePool);
}

template <>
void vk_core::VulkanPipeline<
    window_api_impls::WindowApiFacadeGlfwImpl,
    shader_loader_impls::ShaderLoaderSimpleImpl,
    scene_manager_facades::SceneManagerBaseImpl<
        scene_manager_facades::SceneManagerDependencies>>::recreateSwapChain() {
  auto Size = this->NativeComponents.Facades->Window->ImplInstance.getSize();
  int Width = Size.first, Height = Size.second;
  while (Size.first == 0 || Size.second == 0) {
    glfwGetFramebufferSize((GLFWwindow *)this->NativeComponents.Facades->Window
                               ->ImplInstance.getNativeType(),
                           &Width, &Height);
    this->NativeComponents.Facades->Window->ImplInstance.setSize(
        {Width, Height});
    glfwWaitEvents();
  }

  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice).waitIdle();

  destroySwapChain();
  createSwapChain();
  createFrameBuffers();
  createSemaphores();
  createFences();
  createFrameCommandBuffers();
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
vk::ShaderModule
vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    createShaderModule(std::string ShaderPath) const {
  std::vector<ShaderLoaderFacadeStructs::ShaderData> ShaderData =
      this->NativeComponents.Facades->ShaderLoader->ImplInstance
          .loadAndCompileShaders(ShaderLoaderFacadeStructs::ShadersLocations{
              .Shaders = {
                  {.ShaderName = ShaderPath, .ShaderPath = ShaderPath}}});

  auto SourceCode = ShaderData[0].Data.str();

  vk::ShaderModuleCreateInfo CreateInfo{
      .flags = vk::ShaderModuleCreateFlags(),
      .codeSize = SourceCode.size(),
      .pCode = reinterpret_cast<const uint32_t *>(SourceCode.c_str()),
  };

  try {
    return static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .createShaderModule(CreateInfo);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to create shader module ") +
                             E.what());
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
vk::PipelineLayout
vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                        SceneManagerImplT>::createPipelineLayout() const {
  std::vector<vk::DescriptorSetLayout> Layouts = {
      this->FrameSetLayout,
      this->MeshSetLayout,
  };
  vk::PipelineLayoutCreateInfo CreateInfo{
      .flags = vk::PipelineLayoutCreateFlags(),
      .setLayoutCount = (uint32_t)Layouts.size(),
      .pSetLayouts = Layouts.data(),
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = nullptr,
  };

  try {
    return static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .createPipelineLayout(CreateInfo);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to create pipeline layout ") +
                             E.what());
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
vk::RenderPass
vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                        SceneManagerImplT>::createRenderPass() const {
  std::vector<vk::AttachmentDescription> Attachments{
      {
          // Color
          .flags = vk::AttachmentDescriptionFlags(),
          .format = SwapChainBundle.value().Format,
          .samples = vk::SampleCountFlagBits::e1,
          .loadOp = vk::AttachmentLoadOp::eClear,
          .storeOp = vk::AttachmentStoreOp::eStore,
          .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
          .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
          .initialLayout = vk::ImageLayout::eUndefined,
          .finalLayout = vk::ImageLayout::ePresentSrcKHR,
      },
      {
          // Depth
          .flags = vk::AttachmentDescriptionFlags(),
          .format = SwapChainBundle.value().DepthFormat,
          .samples = vk::SampleCountFlagBits::e1,
          .loadOp = vk::AttachmentLoadOp::eClear,
          .storeOp = vk::AttachmentStoreOp::eStore,
          .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
          .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
          .initialLayout = vk::ImageLayout::eUndefined,
          .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
      },
  };

  std::vector<vk::AttachmentReference> AttachmentRefs{
      {
          .attachment = 0,
          .layout = vk::ImageLayout::eColorAttachmentOptimal,
      },
      {
          .attachment = 1,
          .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
      },
  };

  vk::SubpassDescription Subpass{
      .flags = vk::SubpassDescriptionFlags(),
      .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
      .colorAttachmentCount = 1,
      .pColorAttachments = &AttachmentRefs[0],
      .pDepthStencilAttachment = &AttachmentRefs[1],
  };

  vk::SubpassDependency Dependency{
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
      .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
      .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
      .srcAccessMask = vk::AccessFlagBits::eNoneKHR,
      .dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                       vk::AccessFlagBits::eColorAttachmentWrite,
  };

  vk::RenderPassCreateInfo CreateInfo{
      .flags = vk::RenderPassCreateFlags(),
      .attachmentCount = (uint32_t)Attachments.size(),
      .pAttachments = Attachments.data(),
      .subpassCount = 1,
      .pSubpasses = &Subpass,
      .dependencyCount = 1,
      .pDependencies = &Dependency,
  };

  try {
    return static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .createRenderPass(CreateInfo);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to create render pass ") +
                             E.what());
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    createDescriptorSetLayouts(DescriptorSetLayoutInputStruct LayoutStruct) {
  std::vector<vk::DescriptorSetLayoutBinding> Bindings;
  Bindings.reserve(LayoutStruct.Count);

  for (int I = 0; I < LayoutStruct.Count; ++I) {
    vk::DescriptorSetLayoutBinding LayoutBinding{
        .binding = (uint32_t)LayoutStruct.Indexes[I],
        .descriptorType = LayoutStruct.Types[I],
        .descriptorCount = (uint32_t)LayoutStruct.Counts[I],
        .stageFlags = LayoutStruct.Stages[I],
    };
    Bindings.push_back(LayoutBinding);
  }

  vk::DescriptorSetLayoutCreateInfo CreateInfo{
      .flags = vk::DescriptorSetLayoutCreateFlags(),
      .bindingCount = (uint32_t)LayoutStruct.Count,
      .pBindings = Bindings.data(),
  };

  try {
    this->FrameSetLayout =
        static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
            .createDescriptorSetLayout(CreateInfo);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(
        std::string("Failed to create descriptor set layout ") + E.what());
  }

  LayoutStruct.Count = 1;
  LayoutStruct.Indexes[0] = 0;
  LayoutStruct.Types[0] = vk::DescriptorType::eCombinedImageSampler;
  LayoutStruct.Counts[0] = 1;
  LayoutStruct.Stages[0] = vk::ShaderStageFlagBits::eFragment;

  vk::DescriptorSetLayoutBinding LayoutBinding1{
      .binding = 0,
      .descriptorType = vk::DescriptorType::eCombinedImageSampler,
      .descriptorCount = 1,
      .stageFlags = vk::ShaderStageFlagBits::eFragment,
  };

  std::vector<vk::DescriptorSetLayoutBinding> Bindings1 = {LayoutBinding1};

  vk::DescriptorSetLayoutCreateInfo CreateInfo1{
      .flags = vk::DescriptorSetLayoutCreateFlags(),
      .bindingCount = 1,
      .pBindings = Bindings1.data(),
  };

  try {
    this->MeshSetLayout =
        static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
            .createDescriptorSetLayout(CreateInfo1);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(
        std::string("Failed to create descriptor set layout ") + E.what());
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
vk::DescriptorPool
vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    createDescriptorPool(
        uint32_t Size, VulkanPipeline::DescriptorSetLayoutInputStruct &Input) {
  std::vector<vk::DescriptorPoolSize> PoolSizes;
  for (int I = 0; I < Input.Count; ++I) {
    vk::DescriptorPoolSize PoolSize{
        .type = Input.Types[I],
        .descriptorCount = Size * 10,
    };
    PoolSizes.push_back(PoolSize);
  }

  vk::DescriptorPoolCreateInfo PoolInfo{
      .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
      .maxSets = Size * 10,
      .poolSizeCount = (uint32_t)PoolSizes.size(),
      .pPoolSizes = PoolSizes.data(),
  };

  try {
    return static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .createDescriptorPool(PoolInfo);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to create descriptor pool ") +
                             E.what());
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    writeDescriptorSet(SwapChainFrameStruct &Frame) {
  vk::WriteDescriptorSet WriteDescriptorSet{
      .dstSet = Frame.DescriptorSet,
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = vk::DescriptorType::eUniformBuffer,
      .pBufferInfo = &Frame.UniformBufferDescriptor,
  };
  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
      .updateDescriptorSets(WriteDescriptorSet, nullptr);

  vk::WriteDescriptorSet WriteDescriptorSet1{
      .dstSet = Frame.DescriptorSet,
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = vk::DescriptorType::eStorageBuffer,
      .pBufferInfo = &Frame.ModelBufferDescriptor,
  };
  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
      .updateDescriptorSets(WriteDescriptorSet1, nullptr);
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    createPipeline(VulkanPipeline::GraphicsPipelineInBundle InBundle) {
  vk::GraphicsPipelineCreateInfo CreateInfo;
  CreateInfo.flags = vk::PipelineCreateFlags();

  std::vector<vk::PipelineShaderStageCreateInfo> ShaderStages;

  // Vertex Input
  vk::VertexInputBindingDescription BindingDescription =
      getBindingDescription();
  std::array<vk::VertexInputAttributeDescription, 3> AttributeDescriptions =
      getAttributeDescriptions();
  vk::PipelineVertexInputStateCreateInfo VertexInputInfo = {
      .flags = vk::PipelineVertexInputStateCreateFlags(),
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &BindingDescription,
      .vertexAttributeDescriptionCount = 3,
      .pVertexAttributeDescriptions = AttributeDescriptions.data(),
  };
  CreateInfo.pVertexInputState = &VertexInputInfo;

  // Input Assembly
  vk::PipelineInputAssemblyStateCreateInfo InputAssembly = {
      .flags = vk::PipelineInputAssemblyStateCreateFlags(),
      .topology = vk::PrimitiveTopology::eTriangleList,
      .primitiveRestartEnable = VK_FALSE,
  };
  CreateInfo.pInputAssemblyState = &InputAssembly;

  // Vertex Shader
  vk::ShaderModule VertexShaderModule =
      createShaderModule(InBundle.VertexShaderPath);

  vk::PipelineShaderStageCreateInfo VertexShaderStage = {
      .flags = vk::PipelineShaderStageCreateFlags(),
      .stage = vk::ShaderStageFlagBits::eVertex,
      .module = VertexShaderModule,
      .pName = "main",
  };
  ShaderStages.push_back(VertexShaderStage);

  // Viewport and Scissor
  vk::Viewport Viewport = {
      .x = 0.0f,
      .y = 0.0f,
      .width = static_cast<float>(SwapChainBundle.value().Extent.width),
      .height = static_cast<float>(SwapChainBundle.value().Extent.height),
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };
  vk::Extent2D HalfExtent = {
      .width = SwapChainBundle.value().Extent.width,
      .height = SwapChainBundle.value().Extent.height,
  };
  vk::Rect2D Scissor = {
      .offset = {0, 0},
      .extent = HalfExtent,
  };
  vk::PipelineViewportStateCreateInfo ViewportState = {
      .flags = vk::PipelineViewportStateCreateFlags(),
      .viewportCount = 1,
      .pViewports = &Viewport,
      .scissorCount = 1,
      .pScissors = &Scissor,
  };
  CreateInfo.pViewportState = &ViewportState;

  // Rasterizer
  vk::PipelineRasterizationStateCreateInfo Rasterizer = {
      .flags = vk::PipelineRasterizationStateCreateFlags(),
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = vk::PolygonMode::eFill,
      .cullMode = vk::CullModeFlagBits::eBack,
      .frontFace = vk::FrontFace::eClockwise,
      .depthBiasEnable = VK_FALSE,
      .lineWidth = 1.0f,
  };
  CreateInfo.pRasterizationState = &Rasterizer;

  // Fragment Shader
  vk::ShaderModule FragmentShaderModule =
      createShaderModule(InBundle.FragmentShaderPath);
  vk::PipelineShaderStageCreateInfo FragmentShaderStage = {
      .flags = vk::PipelineShaderStageCreateFlags(),
      .stage = vk::ShaderStageFlagBits::eFragment,
      .module = FragmentShaderModule,
      .pName = "main",
  };
  ShaderStages.push_back(FragmentShaderStage);

  CreateInfo.stageCount = ShaderStages.size();
  CreateInfo.pStages = ShaderStages.data();

  // Depth Stencil
  vk::PipelineDepthStencilStateCreateInfo DepthStencil{
      .flags = vk::PipelineDepthStencilStateCreateFlags(),
      .depthTestEnable = VK_TRUE,
      .depthWriteEnable = VK_TRUE,
      .depthCompareOp = vk::CompareOp::eLess,
      .depthBoundsTestEnable = VK_FALSE,
      .stencilTestEnable = VK_FALSE,
  };
  CreateInfo.pDepthStencilState = &DepthStencil;

  // Multisampling
  vk::PipelineMultisampleStateCreateInfo Multisampling = {
      .flags = vk::PipelineMultisampleStateCreateFlags(),
      .rasterizationSamples = vk::SampleCountFlagBits::e1,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 1.0f,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = VK_FALSE,
      .alphaToOneEnable = VK_FALSE,
  };
  CreateInfo.pMultisampleState = &Multisampling;

  // Color Blend
  vk::PipelineColorBlendAttachmentState ColorBlendAttachment = {
      .blendEnable = VK_FALSE,
      .colorWriteMask =
          vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
  };
  vk::PipelineColorBlendStateCreateInfo ColorBlending = {
      .flags = vk::PipelineColorBlendStateCreateFlags(),
      .logicOpEnable = VK_FALSE,
      .logicOp = vk::LogicOp::eCopy,
      .attachmentCount = 1,
      .pAttachments = &ColorBlendAttachment,
      .blendConstants = vk::ArrayWrapper1D<float, 4>{},
  };
  ColorBlending.blendConstants[0] = 0.0f;
  ColorBlending.blendConstants[1] = 0.0f;
  ColorBlending.blendConstants[2] = 0.0f;
  ColorBlending.blendConstants[3] = 0.0f;
  CreateInfo.pColorBlendState = &ColorBlending;

  // Pipeline Layout
  CreateInfo.layout = createPipelineLayout();

  // Renderpass
  CreateInfo.renderPass = createRenderPass();

  // Extra stuff
  CreateInfo.basePipelineHandle = nullptr;

  //  std::vector<vk::DynamicState> DynamicStates = {
  //      vk::DynamicState::eViewport,
  //      vk::DynamicState::eScissor,
  //  };
  //  vk::PipelineDynamicStateCreateInfo DynamicState{};
  //  DynamicState.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
  //  DynamicState.dynamicStateCount =
  //  static_cast<uint32_t>(DynamicStates.size()); DynamicState.pDynamicStates =
  //  DynamicStates.data(); CreateInfo.pDynamicState = &DynamicState;

  // Create Pipeline
  vk::Pipeline Pipeline;
  try {
    Pipeline = static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
                   .createGraphicsPipeline(nullptr, CreateInfo)
                   .value;
  } catch (vk::SystemError &E) {
    throw std::runtime_error(
        std::string("Failed to create graphics pipeline ") + E.what());
  }

  // Init Result
  this->PipelineBundle = GraphicsPipelineOutBundle{
      .Pipeline = Pipeline,
      .Layout = CreateInfo.layout,
      .RenderPass = CreateInfo.renderPass,
  };

  // Destroy Shader Modules
  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
      .destroyShaderModule(VertexShaderModule);
  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
      .destroyShaderModule(FragmentShaderModule);

  LOG_F(INFO, "Pipeline created successfully");
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                             SceneManagerImplT>::createFrameBuffers() {
  for (int I = 0; I < this->SwapChainBundle->Frames.size(); ++I) {
    std::vector<vk::ImageView> Attachments = {
        this->SwapChainBundle->Frames[I].ImageView,
        this->SwapChainBundle->Frames[I].DepthBufferView,
    };

    vk::FramebufferCreateInfo FramebufferCreateInfo{
        .flags = vk::FramebufferCreateFlags(),
        .renderPass = this->PipelineBundle->RenderPass,
        .attachmentCount = static_cast<uint32_t>(Attachments.size()),
        .pAttachments = Attachments.data(),
        .width = this->SwapChainBundle->Extent.width,
        .height = this->SwapChainBundle->Extent.height,
        .layers = 1,
    };

    try {
      this->SwapChainBundle->Frames[I].FrameBuffer =
          static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
              .createFramebuffer(FramebufferCreateInfo);
    } catch (vk::SystemError &E) {
      throw std::runtime_error(std::string("Failed to create framebuffer ") +
                               E.what());
    }
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                             SceneManagerImplT>::createCommandPool() {
  auto Indexes = this->NativeComponents.FamilyIndexes;

  vk::CommandPoolCreateInfo CreateInfo{
      .flags = vk::CommandPoolCreateFlags() |
               vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      .queueFamilyIndex = Indexes[vk::QueueFlagBits::eGraphics],
  };

  try {
    this->CommandPool =
        static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
            .createCommandPool(CreateInfo);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to create command pool ") +
                             E.what());
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                             SceneManagerImplT>::createMainCommandBuffer() {
  vk::CommandBufferAllocateInfo AllocateInfo{
      .commandPool = this->CommandPool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount =
          static_cast<uint32_t>(this->SwapChainBundle->Frames.size()),
  };

  try {
    this->MainCommandBuffer =
        static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
            .allocateCommandBuffers(AllocateInfo)[0];
  } catch (vk::SystemError &E) {
    throw std::runtime_error(
        std::string("Failed to allocate main command buffer") + E.what());
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                             SceneManagerImplT>::createFrameCommandBuffers() {
  vk::CommandBufferAllocateInfo AllocateInfo{
      .commandPool = this->CommandPool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount =
          static_cast<uint32_t>(this->SwapChainBundle->Frames.size()),
  };

  for (auto &Frame : this->SwapChainBundle->Frames) {
    try {
      Frame.CommandBuffer =
          static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
              .allocateCommandBuffers(AllocateInfo)[0];
    } catch (vk::SystemError &E) {
      throw std::runtime_error(
          std::string("Failed to allocate command buffers ") + E.what());
    }
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                             SceneManagerImplT>::createSemaphores() {
  vk::SemaphoreCreateInfo CreateInfo{};

  try {
    for (auto &Frame : this->SwapChainBundle->Frames) {
      Frame.ImageAvailableSemaphore =
          static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
              .createSemaphore(CreateInfo);
      Frame.RenderFinishedSemaphore =
          static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
              .createSemaphore(CreateInfo);
    }
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to create semaphores ") +
                             E.what());
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                             SceneManagerImplT>::createFences() {
  vk::FenceCreateInfo CreateInfo{.flags = vk::FenceCreateFlags() |
                                          vk::FenceCreateFlagBits::eSignaled};

  DescriptorSetLayoutInputStruct DescriptorSetLayoutInput{
      .Count = 2,
      .Types = {vk::DescriptorType::eUniformBuffer,
                vk::DescriptorType::eStorageBuffer},
  };
  FramePool = createDescriptorPool(this->SwapChainBundle->Frames.size(),
                                   DescriptorSetLayoutInput);

  for (auto &Frame : this->SwapChainBundle->Frames) {
    try {
      Frame.InFlightFence =
          static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
              .createFence(CreateInfo);
    } catch (vk::SystemError &E) {
      throw std::runtime_error(std::string("Failed to create fence ") +
                               E.what());
    }

    createDescriptorBuffer(Frame);
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    recordDrawCommands(vk::CommandBuffer CommandBuffer, uint32_t ImageIndex) {
  vk::CommandBufferBeginInfo BeginInfo;

  try {
    CommandBuffer.begin(BeginInfo);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(
        std::string("Failed to begin recording main command buffer ") +
        E.what());
  }

  vk::ClearValue Clear{std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f}};
  vk::ClearValue DepthClear;
  DepthClear.depthStencil = vk::ClearDepthStencilValue{1.0f, 0};
  std::vector<vk::ClearValue> ClearValues{{
      Clear,
      DepthClear,
  }};
  vk::RenderPassBeginInfo RenderPassInfo{
      .renderPass = this->PipelineBundle->RenderPass,
      .framebuffer = this->SwapChainBundle->Frames[ImageIndex].FrameBuffer,
      .renderArea =
          {
              .offset = {0, 0},
              .extent = this->SwapChainBundle->Extent,
          },
      .clearValueCount = (uint32_t)ClearValues.size(),
      .pClearValues = ClearValues.data(),
  };

  CommandBuffer.beginRenderPass(&RenderPassInfo, vk::SubpassContents::eInline);
  CommandBuffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics, this->PipelineBundle->Layout, 0,
      this->SwapChainBundle->Frames[ImageIndex].DescriptorSet, nullptr);
  CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                             this->PipelineBundle->Pipeline);

  auto *SceneManager =
      &this->NativeComponents.Facades->SceneManager->ImplInstance;

  SceneManager->resetObjectGetter();
  uint32_t Offset = 0;
  while (true) {
    SceneManagerFacadeStructs::OneTypeObjects Objects =
        SceneManager->getCurrentObjects();

    // Current type vector is empty (or not even created)
    if (Objects.empty()) {
      if (SceneManager->goToNextDump()) {
        // Step to next dump
        Offset = 0;
        Objects = SceneManager->getCurrentObjects();
        drawObjects(Objects, CommandBuffer, Offset);
        Offset += Objects.size();
        continue;
      }
      break;
    }

    // if all ok
    drawObjects(Objects, CommandBuffer, Offset);
    Offset += Objects.size();
    const auto &Discard = SceneManager->getNextObjects();
  }

  auto [width, height] =
      this->NativeComponents.Facades->Window->ImplInstance.getSize();
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::ShowDemoWindow();

  ImGui::SetNextWindowSize(ImVec2(350, height));
  ImGui::SetNextWindowPos(ImVec2(0, 0)); // Set the position of the new window

  bool ShowDialog = true;
  ImGui::Begin("Objects", &ShowDialog, ImGuiWindowFlags_NoCollapse);

  //  ImGui::PushID(0ß);
  if (ImGui::TreeNode("Root")) {
    int I = 0;
    if (ImGui::TreeNode((void *)&I, "Root")) {
      ImGui::Text("Hello world");
      ImGui::TreePop();
    }
    ImGui::TreePop();
  }

  if (ImGui::BeginPopup("hiu")) {
    ImGui::Text("test");
    ImGui::EndPopup();
  }
  ImGui::OpenPopup("hiu");

  //  ImGui::PopID();
  //  //ImGui::TreePop();
  ImGui::End();

  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), CommandBuffer);

  CommandBuffer.endRenderPass();

  try {
    CommandBuffer.end();
  } catch (vk::SystemError &E) {
    throw std::runtime_error(
        std::string("Failed to end recording main command buffer ") + E.what());
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    drawObjects(SceneManagerFacadeStructs::OneTypeObjects Objects,
                vk::CommandBuffer CommandBuffer, uint32_t Offset) {
  // Iterate through textures
  for (auto &[TextureName, ObjectsSet] : Objects) {
    std::string DumpName = ObjectsSet[0]->getDumpName();
    prepareScene(CommandBuffer, MeshTypes.Dumps[DumpName]);

    BaseObject::ObjectTypes Type = ObjectsSet[0]->getType();
    std::string TypeName = BaseObject::toString(Type);

    this->Textures[TextureName]->use(CommandBuffer,
                                     this->PipelineBundle->Layout);
    CommandBuffer.drawIndexed(
        this->MeshTypes.Dumps[DumpName].Meshes[TypeName].IndexCount,
        Objects.size(),
        this->MeshTypes.Dumps[DumpName].Meshes[TypeName].FirstIndex, 0, Offset);
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    createBuffer(BufferInput Input, MeshDump &MeshData) {
  vk::BufferCreateInfo BufferInfo{
      .flags = vk::BufferCreateFlags(),
      .size = Input.Size,
      .usage = Input.Usage,
      .sharingMode = vk::SharingMode::eExclusive,
  };

  try {
    MeshData.Buffer =
        static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
            .createBuffer(BufferInfo);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to create buffer ") +
                             E.what());
  }

  allocateBufferMemory(MeshData);
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    copyBuffer(vk::Buffer SrcBuffer, vk::Buffer DstBuffer, vk::DeviceSize Size,
               vk::Queue Queue, vk::CommandBuffer CommandBuffer) {
  CommandBuffer.reset();

  vk::CommandBufferBeginInfo BeginInfo{
      .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
  };
  CommandBuffer.begin(BeginInfo);

  vk::BufferCopy CopyRegion{
      .srcOffset = 0,
      .dstOffset = 0,
      .size = Size,
  };
  CommandBuffer.copyBuffer(SrcBuffer, DstBuffer, CopyRegion);
  CommandBuffer.end();

  vk::SubmitInfo SubmitInfo{
      .commandBufferCount = 1,
      .pCommandBuffers = &CommandBuffer,
  };
  const auto &Discard = Queue.submit(1, &SubmitInfo, nullptr);
  Queue.waitIdle();
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
uint32_t
vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    findMemoryType(uint32_t TypeFilter, vk::MemoryPropertyFlags Properties) {
  vk::PhysicalDeviceMemoryProperties MemoryProperties =
      static_cast<vk::PhysicalDevice &>(*this->NativeComponents.PhysicalDevice)
          .getMemoryProperties();

  for (uint32_t I = 0; I < MemoryProperties.memoryTypeCount; ++I) {
    const auto Supported = TypeFilter & (1 << I);
    const auto Sufficient = (MemoryProperties.memoryTypes[I].propertyFlags &
                             Properties) == Properties;
    if (Supported && Sufficient) {
      return I;
    }
  }
  throw std::runtime_error("Failed to find suitable memory type");
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                             SceneManagerImplT>::render() {
  static bool Once = false;
  if (!Once) {
    initUi();
    Once = true;
  }

  auto &Device =
      static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice);

  const auto &Discard1 = Device.waitForFences(
      1, &SwapChainBundle->Frames[FrameNumber].InFlightFence, VK_TRUE,
      UINT64_MAX);
  const auto &Discard2 = Device.resetFences(
      1, &SwapChainBundle->Frames[FrameNumber].InFlightFence);

  uint32_t ImageIndex;
  try {
    vk::ResultValue Acquire = Device.acquireNextImageKHR(
        SwapChainBundle->Swapchain, UINT64_MAX,
        SwapChainBundle->Frames[FrameNumber].ImageAvailableSemaphore, nullptr);

    ImageIndex = Acquire.value;
  } catch (vk::OutOfDateKHRError &E) {
    recreateSwapChain();
    return;
  } catch (vk::IncompatibleDisplayKHRError &E) {
    recreateSwapChain();
    return;
  } catch (vk::SystemError &E) {
    throw std::runtime_error(
        std::string("Failed to acquire swap chain image ") + E.what());
  }

  vk::CommandBuffer CommandBuffer =
      SwapChainBundle->Frames[ImageIndex].CommandBuffer;
  CommandBuffer.reset();

  prepareFrame(ImageIndex);

  recordDrawCommands(CommandBuffer, ImageIndex);

  vk::Semaphore WaitSemaphores[] = {
      SwapChainBundle->Frames[FrameNumber].ImageAvailableSemaphore};
  vk::PipelineStageFlags WaitStages[] = {
      vk::PipelineStageFlagBits::eColorAttachmentOutput};
  vk::Semaphore SignalSemaphores[] = {
      SwapChainBundle->Frames[FrameNumber].RenderFinishedSemaphore};
  vk::SubmitInfo SubmitInfo{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores =
          &SwapChainBundle->Frames[FrameNumber].ImageAvailableSemaphore,
      .pWaitDstStageMask = WaitStages,
      .commandBufferCount = 1,
      .pCommandBuffers = &CommandBuffer,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = SignalSemaphores,
  };

  try {
    this->NativeComponents.GraphicsQueue->submit(
        SubmitInfo, SwapChainBundle->Frames[FrameNumber].InFlightFence);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to submit draw command ") +
                             E.what());
  }

  vk::SwapchainKHR SwapChains[] = {this->SwapChainBundle->Swapchain};
  vk::PresentInfoKHR PresentInfo{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = SignalSemaphores,
      .swapchainCount = 1,
      .pSwapchains = SwapChains,
      .pImageIndices = &ImageIndex,
  };

  vk::Result PresentResult;
  try {
    PresentResult =
        this->NativeComponents.PresentQueue->presentKHR(PresentInfo);
  } catch (vk::OutOfDateKHRError &E) {
    PresentResult = vk::Result::eErrorOutOfDateKHR;
  }

  if (PresentResult == vk::Result::eErrorOutOfDateKHR ||
      PresentResult == vk::Result::eSuboptimalKHR) {
    recreateSwapChain();
    return;
  }

  FrameNumber = (FrameNumber + 1) % MaxFrameInFlight;
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    allocateBufferMemory(VulkanPipeline::MeshDump &Mesh) {
  vk::MemoryRequirements MemoryRequirements =
      static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
          .getBufferMemoryRequirements(Mesh.Buffer);

  vk::MemoryAllocateInfo AllocateInfo{
      .allocationSize = MemoryRequirements.size,
      .memoryTypeIndex =
          findMemoryType(MemoryRequirements.memoryTypeBits,
                         vk::MemoryPropertyFlagBits::eHostVisible |
                             vk::MemoryPropertyFlagBits::eHostCoherent),
  };

  Mesh.Memory =
      static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
          .allocateMemory(AllocateInfo);
  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
      .bindBufferMemory(Mesh.Buffer, Mesh.Memory, 0);
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    allocateDescriptorSet(VulkanPipeline::SwapChainFrameStruct &Frame) {
  vk::DescriptorSetAllocateInfo AllocateInfo{
      .descriptorPool = this->FramePool,
      .descriptorSetCount = 1,
      .pSetLayouts = &this->FrameSetLayout,
  };

  try {
    Frame.DescriptorSet =
        static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
            .allocateDescriptorSets(AllocateInfo)[0];
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to allocate descriptor set ") +
                             E.what());
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
vk::VertexInputBindingDescription
vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                        SceneManagerImplT>::getBindingDescription() {
  vk::VertexInputBindingDescription BindingDescription{
      .binding = 0,
      .stride = 7 * sizeof(float),
      .inputRate = vk::VertexInputRate::eVertex,
  };

  return BindingDescription;
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
std::array<vk::VertexInputAttributeDescription, 3>
vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                        SceneManagerImplT>::getAttributeDescriptions() {
  std::array<vk::VertexInputAttributeDescription, 3> Attributes{
      vk::VertexInputAttributeDescription{
          .location = 0,
          .binding = 0,
          .format = vk::Format::eR32G32Sfloat,
          .offset = 0,
      },
      vk::VertexInputAttributeDescription{
          .location = 1,
          .binding = 0,
          .format = vk::Format::eR32G32B32Sfloat,
          .offset = 2 * sizeof(float),
      },
      vk::VertexInputAttributeDescription{
          .location = 2,
          .binding = 0,
          .format = vk::Format::eR32G32Sfloat,
          .offset = 5 * sizeof(float),
      },
  };

  return Attributes;
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    addObjectData(
        const std::map<std::string, VulkanPipeline::PublicObjectData> &Dump,
        const std::string &DumpName) {
  MeshDump StagingDump{
      .Buffer = nullptr,
      .Memory = nullptr,
      .Meshes = {},
  };

  int Offset = 0;
  int IndexOffset = 0;

  // Create Dump of objects
  for (auto &[Name, ObjectData] : Dump) {
    std::vector<float> MeshArray;
    for (int IVertex = 0, IColor = 0, ITexCoord = 0;
         IVertex < ObjectData.Vertices.size();
         IVertex += 2, IColor += 3, ITexCoord += 2) {
      MeshArray.push_back(ObjectData.Vertices[IVertex + 0]);
      MeshArray.push_back(ObjectData.Vertices[IVertex + 1]);
      MeshArray.push_back(ObjectData.Colors[IColor + 0]);
      MeshArray.push_back(ObjectData.Colors[IColor + 1]);
      MeshArray.push_back(ObjectData.Colors[IColor + 2]);
      MeshArray.push_back(ObjectData.TexCoords[ITexCoord + 0]);
      MeshArray.push_back(ObjectData.TexCoords[ITexCoord + 1]);
    }

    size_t ArraySize = MeshArray.size();
    MeshData NewMeshData{
        .Data = std::move(MeshArray),
        .Indexes = ObjectData.Indexes,
        .Offset = Offset,
        .Size = static_cast<int>(ArraySize / 7),
        .IndexCount = (uint32_t)ObjectData.Indexes.size(),
        .FirstIndex = (uint32_t)IndexOffset,
    };

    Offset += NewMeshData.Size;
    IndexOffset += NewMeshData.IndexCount;

    StagingDump.Meshes.emplace(Name, std::move(NewMeshData));
  }

  StagingDump.recalculateDataCache();

  BufferInput BufferInput{
      .Size = StagingDump.DataCache.size() * sizeof(float),
      .Usage = vk::BufferUsageFlagBits::eTransferSrc |
               vk::BufferUsageFlagBits::eVertexBuffer,
      .Properties = vk::MemoryPropertyFlagBits::eHostVisible |
                    vk::MemoryPropertyFlagBits::eHostCoherent,
  };

  MeshDump NewMeshDump{
      .Buffer = nullptr,
      .Memory = nullptr,
      .Meshes = {},
  };

  createBuffer(BufferInput, StagingDump);

  void *MemoryLocation =
      static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
          .mapMemory(StagingDump.Memory, 0, BufferInput.Size);
  std::memcpy(MemoryLocation, StagingDump.DataCache.data(), BufferInput.Size);
  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
      .unmapMemory(StagingDump.Memory);

  NewMeshDump.Meshes = std::move(StagingDump.Meshes);

  BufferInput.Usage = vk::BufferUsageFlagBits::eTransferDst |
                      vk::BufferUsageFlagBits::eVertexBuffer;
  BufferInput.Properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
  createBuffer(BufferInput, NewMeshDump);

  copyBuffer(StagingDump.Buffer, NewMeshDump.Buffer, BufferInput.Size,
             *this->NativeComponents.GraphicsQueue, this->MainCommandBuffer);

  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
      .destroyBuffer(StagingDump.Buffer);
  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
      .freeMemory(StagingDump.Memory);

  // Add Dump and calculate result buffer for buffer creation
  MeshTypes.Dumps.emplace(DumpName, std::move(NewMeshDump));

  BufferInput.Size = StagingDump.IndexLump.size() * sizeof(uint32_t);
  BufferInput.Usage = vk::BufferUsageFlagBits::eTransferSrc |
                      vk::BufferUsageFlagBits::eVertexBuffer;
  BufferInput.Properties = vk::MemoryPropertyFlagBits::eHostVisible |
                           vk::MemoryPropertyFlagBits::eHostCoherent;

  NewMeshDump.Buffer = nullptr;
  NewMeshDump.Memory = nullptr;
  NewMeshDump.Meshes = {};

  createBuffer(BufferInput, StagingDump);

  MemoryLocation =
      static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
          .mapMemory(StagingDump.Memory, 0, BufferInput.Size);
  std::memcpy(MemoryLocation, StagingDump.IndexLump.data(), BufferInput.Size);
  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
      .unmapMemory(StagingDump.Memory);

  BufferInput.Usage = vk::BufferUsageFlagBits::eTransferDst |
                      vk::BufferUsageFlagBits::eIndexBuffer;
  BufferInput.Properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
  createBuffer(BufferInput, NewMeshDump);

  copyBuffer(StagingDump.Buffer, NewMeshDump.Buffer, BufferInput.Size,
             *this->NativeComponents.GraphicsQueue, this->MainCommandBuffer);

  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
      .destroyBuffer(StagingDump.Buffer);
  static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
      .freeMemory(StagingDump.Memory);

  MeshTypes.Dumps[DumpName].IndexBuffer = NewMeshDump.Buffer;
  MeshTypes.Dumps[DumpName].IndexMemory = NewMeshDump.Memory;
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    prepareScene(vk::CommandBuffer CommandBuffer, MeshDump &Dump) {
  vk::Buffer Buffers[] = {Dump.Buffer};
  vk::DeviceSize Offsets[] = {0};
  CommandBuffer.bindVertexBuffers(0, 1, Buffers, Offsets);
  CommandBuffer.bindIndexBuffer(Dump.IndexBuffer, 0, vk::IndexType::eUint32);
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    addTexture(const std::string &TexturePath, const std::string &TextureName) {
  DescriptorSetLayoutInputStruct DescriptorSetLayoutInput{
      .Count = 1,
      .Types = {vk::DescriptorType::eCombinedImageSampler},
  };
  MeshPool = createDescriptorPool(1, DescriptorSetLayoutInput);
  VkTexture::TextureInputChunk TextureInput{
      .LogicalDevice =
          static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice),
      .PhysicalDevice = static_cast<vk::PhysicalDevice &>(
          *this->NativeComponents.PhysicalDevice),
      .CommandBuffer = this->MainCommandBuffer,
      .Queue = *this->NativeComponents.GraphicsQueue,
      .DescriptorSetLayout = this->MeshSetLayout,
      .DescriptorPool = this->MeshPool,
      .FileName = TexturePath,
  };
  this->Textures[TextureName] = new VkTexture(TextureInput);
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    prepareFrame(uint32_t ImageIndex) {
  SwapChainFrameStruct &Frame = SwapChainBundle->Frames[ImageIndex];
  auto [Width, Height] =
      this->NativeComponents.Facades->Window->ImplInstance.getSize();

  // Get Camera data
  SceneManagerFacadeStructs::CameraData CameraData =
      this->NativeComponents.Facades->SceneManager->ImplInstance.getCamera(
          {Width, Height});
  std::memcpy(Frame.CameraDataWriteLocation, &CameraData,
              sizeof(SceneManagerFacadeStructs::CameraData));

  std::vector<glm::mat4> Transformations =
      this->NativeComponents.Facades->SceneManager->ImplInstance
          .getTransformations();
  for (int I = 0; I < Transformations.size(); ++I) {
    Frame.ModelTransforms[I] = Transformations[I];
  }

  std::memcpy(Frame.ModelDataWriteLocation, Frame.ModelTransforms.data(),
              Transformations.size() * sizeof(glm::mat4));

  writeDescriptorSet(Frame);
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    createDescriptorBuffer(SwapChainFrameStruct &Frame) {
  BufferInput BufferInputInstance{
      .Size = sizeof(SceneManagerFacadeStructs::CameraData),
      .Usage = vk::BufferUsageFlagBits::eUniformBuffer,
      .Properties = vk::MemoryPropertyFlagBits::eHostVisible |
                    vk::MemoryPropertyFlagBits::eHostCoherent,
  };
  MeshDump CameraData{
      .Buffer = nullptr,
      .Memory = nullptr,
      .Meshes = {},
      .DataCache = {},
  };
  createBuffer(BufferInputInstance, CameraData);

  Frame.CameraBuffer = CameraData.Buffer;
  Frame.CameraMemory = CameraData.Memory;
  Frame.CameraDataWriteLocation =
      static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
          .mapMemory(Frame.CameraMemory, 0, BufferInputInstance.Size);

  BufferInputInstance.Size = 1024 * sizeof(glm::mat4);
  BufferInputInstance.Usage = vk::BufferUsageFlagBits::eStorageBuffer;

  MeshDump UniformBuffer{
      .Buffer = nullptr,
      .Memory = nullptr,
      .Meshes = {},
      .DataCache = {},
  };
  createBuffer(BufferInputInstance, UniformBuffer);

  Frame.ModelBuffer = UniformBuffer.Buffer;
  Frame.ModelMemory = UniformBuffer.Memory;
  Frame.ModelDataWriteLocation =
      static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
          .mapMemory(Frame.ModelMemory, 0, BufferInputInstance.Size);

  Frame.ModelTransforms.reserve(1024);
  for (int I = 0; I < 1024; ++I) {
    Frame.ModelTransforms.push_back(glm::mat4(1.0f));
    Frame.ModelTransforms.push_back(glm::mat4(1.0f));
  }

  Frame.UniformBufferDescriptor.buffer = Frame.CameraBuffer;
  Frame.UniformBufferDescriptor.offset = 0;
  Frame.UniformBufferDescriptor.range =
      sizeof(SceneManagerFacadeStructs::CameraData);

  Frame.ModelBufferDescriptor.buffer = Frame.ModelBuffer;
  Frame.ModelBufferDescriptor.offset = 0;
  Frame.ModelBufferDescriptor.range = 1024 * sizeof(glm::mat4);

  allocateDescriptorSet(Frame);
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                             SceneManagerImplT>::initUi() {
  ImGui::CreateContext();
  ImGui::GetIO().ConfigFlags |=
      ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard;

  GLFWwindow *Window = (GLFWwindow *)this->NativeComponents.Facades->Window
                           ->ImplInstance.getNativeType();
  ImGui_ImplGlfw_InitForVulkan(Window, true);

  ImGui_ImplVulkan_InitInfo Info{
      .Instance = static_cast<vk::Instance &>(*this->NativeComponents.Instance),
      .PhysicalDevice = static_cast<vk::PhysicalDevice>(
          *this->NativeComponents.PhysicalDevice),
      .Device = static_cast<vk::Device>(*this->NativeComponents.PhysicalDevice),
      .QueueFamily =
          this->NativeComponents.FamilyIndexes[vk::QueueFlagBits::eGraphics],
      .Queue = static_cast<VkQueue>(*this->NativeComponents.GraphicsQueue),
      .DescriptorPool = this->MeshPool,
      .MinImageCount = 2,
      .ImageCount = (uint32_t)this->SwapChainBundle->Frames.size(),
      .MSAASamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
      .PipelineCache = nullptr,
      .UseDynamicRendering = false,
      .Allocator = nullptr,
      .CheckVkResultFn = nullptr,
  };
  ImGui_ImplVulkan_Init(&Info, this->PipelineBundle->RenderPass);

  VkCommandBufferAllocateInfo AllocInfo{};
  AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  AllocInfo.commandPool = CommandPool;
  AllocInfo.commandBufferCount = 1;

  VkDevice Device =
      static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice);
  VkCommandBuffer CommandBuffer;
  vkAllocateCommandBuffers(Device, &AllocInfo, &CommandBuffer);

  VkCommandBufferBeginInfo BeginInfo{};
  BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

  ImGui_ImplVulkan_CreateFontsTexture(CommandBuffer);

  vkEndCommandBuffer(CommandBuffer);

  VkSubmitInfo SubmitInfo{};
  SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  SubmitInfo.commandBufferCount = 1;
  SubmitInfo.pCommandBuffers = &CommandBuffer;

  VkQueue GraphicsQueue =
      static_cast<VkQueue>(*this->NativeComponents.GraphicsQueue);
  vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(GraphicsQueue);

  vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);

  vkDeviceWaitIdle(Device);
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
vk::Format VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    findSupportedFormat(const std::vector<vk::Format> &Candidates,
                        vk::ImageTiling Tiling,
                        vk::FormatFeatureFlags Features) {
  for (vk::Format Format : Candidates) {
    vk::FormatProperties Props = static_cast<vk::PhysicalDevice &>(
                                     *this->NativeComponents.PhysicalDevice)
                                     .getFormatProperties(Format);

    if (Tiling == vk::ImageTiling::eLinear &&
        (Props.linearTilingFeatures & Features) == Features) {
      return Format;
    }

    if (Tiling == vk::ImageTiling::eOptimal &&
        (Props.optimalTilingFeatures & Features) == Features) {
      return Format;
    }
  }

  throw std::runtime_error("Failed to find supported format");
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    CreateDepthResources(SwapChainFrameStruct &Frame) {
  Frame.DepthFormat = findSupportedFormat(
      {
          vk::Format::eD32Sfloat,
          vk::Format::eD24UnormS8Uint,
      },
      vk::ImageTiling::eOptimal,
      vk::FormatFeatureFlagBits::eDepthStencilAttachment);

  auto [Width, Height] =
      this->NativeComponents.Facades->Window->ImplInstance.getSize();
  VkTexture::ImageInputChunk ImageCreateInfo{
      .LogicalDevice =
          static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice),
      .PhysicalDevice = static_cast<vk::PhysicalDevice &>(
          *this->NativeComponents.PhysicalDevice),
      .Size = {SwapChainBundle->Extent.width, SwapChainBundle->Extent.height},
      .Tiling = vk::ImageTiling::eOptimal,
      .Usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
      .MemoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
      .Format = Frame.DepthFormat,
  };
  Frame.DepthBuffer = VkTexture::createImage(ImageCreateInfo);
  Frame.DepthBufferMemory =
      VkTexture::createImageMemory(ImageCreateInfo, Frame.DepthBuffer);
  Frame.DepthBufferView = VkTexture::createImageView(
      static_cast<vk::Device>(*this->NativeComponents.PhysicalDevice),
      Frame.DepthBuffer, Frame.DepthFormat, vk::ImageAspectFlagBits::eDepth);
}

template class vk_core::VulkanPipeline<
    window_api_impls::WindowApiFacadeGlfwImpl,
    shader_loader_impls::ShaderLoaderSimpleImpl,
    scene_manager_facades::SceneManagerBaseImpl<
        scene_manager_facades::SceneManagerDependencies>>;
