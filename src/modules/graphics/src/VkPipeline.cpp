//
// Created by FullHat on 06/05/2024.
//

#include "VkPipeline.h"

#include "loguru/loguru.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <set>

using namespace vk_core;

template <WindowApiImpl WindowImpl>
vk_core::VkPipeline<WindowImpl>::VkPipeline(VkPipeline::VkPipelineProps Props) {
  this->NativeComponents.PhysicalDevice = Props.PhysicalDevice;
  this->NativeComponents.Instance = Props.Instance;
  this->NativeComponents.Facades = {.Window = Props.Facades.Window};
  this->NativeComponents.FamilyIndexes = std::move(Props.FamilyIndexes);

  initWindowSurface();
  createSwapChain();
  LOG_F(INFO, "Init pipeline, GOOD");
}

template <WindowApiImpl WindowImpl>
vk_core::VkPipeline<WindowImpl>::~VkPipeline() {
  if (this->NativeComponents.PhysicalDevice == nullptr) {
    LOG_F(ERROR, "Device is null");
  }

  for (auto &Frame : SwapChainBundle.value().Frames) {
    static_cast<vk::Device &>(*this->NativeComponents.PhysicalDevice)
        .destroyImageView(Frame.ImageView);
  }

  static_cast<vk::Device &>(*NativeComponents.PhysicalDevice)
      .destroySwapchainKHR(SwapChainBundle.value().Swapchain);

  static_cast<vk::Instance &>(*NativeComponents.Instance)
      .destroySurfaceKHR(NativeComponents.Surface.value());
  NativeComponents.Surface.reset();
}

template <>
void vk_core::VkPipeline<
    window_api_impls::WindowApiFacadeGlfwImpl>::initWindowSurface() {
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

template <WindowApiImpl WindowImpl>
void vk_core::VkPipeline<WindowImpl>::querySwapChainSupport() {
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

template <WindowApiImpl WindowImpl>
vk::SurfaceFormatKHR
vk_core::VkPipeline<WindowImpl>::chooseSwapChainSurfaceFormat() {
  for (vk::SurfaceFormatKHR Format :
       this->NativeComponents.SwapChainSupport.value().Formats) {
    if (Format.format == vk::Format::eB8G8R8A8Unorm &&
        Format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
      return Format;
    }
  }

  return this->NativeComponents.SwapChainSupport.value().Formats[0];
}

template <WindowApiImpl WindowImpl>
vk::PresentModeKHR
vk_core::VkPipeline<WindowImpl>::chooseSwapChainPresentFormat() {
  for (vk::PresentModeKHR PresentMode :
       this->NativeComponents.SwapChainSupport.value().PresentModes) {
    if (PresentMode == vk::PresentModeKHR::eMailbox) {
      return PresentMode;
    }
  }

  return vk::PresentModeKHR::eFifo;
}

template <WindowApiImpl WindowImpl>
vk::Extent2D vk_core::VkPipeline<WindowImpl>::chooseSwapChainExtent(
    std::pair<uint32_t, uint32_t> SizeWH) {
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

template <WindowApiImpl WindowImpl>
void vk_core::VkPipeline<WindowImpl>::logSwapChainInfo() {
  auto &Support = this->NativeComponents.SwapChainSupport.value();
  LOG_F(INFO, "Swapchain can support the following surface capabilities:\n");

  LOG_F(INFO, "\tminimum image count: %d\n",
        Support.Capabilities.minImageCount);
  LOG_F(INFO, "\tmaximum image count: %d\n",
        Support.Capabilities.maxImageCount);

  LOG_F(INFO, "\tcurrent extent: \n");

  LOG_F(INFO, "\t\twidth: %d\n", Support.Capabilities.currentExtent.width);
  LOG_F(INFO, "\t\theight: %d\n", Support.Capabilities.currentExtent.height);

  LOG_F(INFO, "\tminimum supported extent: \n");
  LOG_F(INFO, "\t\twidth: %d\n", Support.Capabilities.minImageExtent.width);
  LOG_F(INFO, "\t\theight: %d\n", Support.Capabilities.minImageExtent.height);

  LOG_F(INFO, "\tmaximum supported extent: \n");
  LOG_F(INFO, "\t\twidth: %d\n", Support.Capabilities.maxImageExtent.width);
  LOG_F(INFO, "\t\theight: %d\n", Support.Capabilities.maxImageExtent.height);
  LOG_F(INFO, "\tmaximum image array layers: %d\n",
        Support.Capabilities.maxImageArrayLayers);

  LOG_F(INFO, "\tsupported transforms:\n");
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

template <WindowApiImpl WindowImpl>
void vk_core::VkPipeline<WindowImpl>::logPresentMode(vk::PresentModeKHR Mode) {
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

template <WindowApiImpl WindowImpl>
void vk_core::VkPipeline<WindowImpl>::logTransformBits(
    vk::SurfaceTransformFlagsKHR Transform) {
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

template <WindowApiImpl WindowImpl>
void vk_core::VkPipeline<WindowImpl>::logAlphaBits(
    vk::CompositeAlphaFlagsKHR Alpha) {
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

template <WindowApiImpl WindowImpl>
void vk_core::VkPipeline<WindowImpl>::logImageBits(
    vk::ImageUsageFlags ImageFlags) {
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

template <WindowApiImpl WindowImpl>
void vk_core::VkPipeline<WindowImpl>::createSwapChain() {
  querySwapChainSupport();
  auto Format = chooseSwapChainSurfaceFormat();
  auto PresentMode = chooseSwapChainPresentFormat();
  auto Extent = chooseSwapChainExtent({1, 2});

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
}

template class vk_core::VkPipeline<window_api_impls::WindowApiFacadeGlfwImpl>;
