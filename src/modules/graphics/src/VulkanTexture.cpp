//
// Created by FullHat on 19/05/2024.
//

#include "VulkanTexture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

vk_core::VulkanTexture::VulkanTexture(
    vk_core::VulkanTexture::TextureInputChunk InputChunk) {
  LogicalDevice = InputChunk.LogicalDevice;
  PhysicalDevice = InputChunk.PhysicalDevice;
  CommandBuffer = InputChunk.CommandBuffer;
  Queue = InputChunk.Queue;
  DescriptorSetLayout = InputChunk.DescriptorSetLayout;
  DescriptorPool = InputChunk.DescriptorPool;

  for (const auto &FileName : InputChunk.FileNames) {
    auto NewTexture = TextureData{};
    NewTexture.FileName = FileName;

    NewTexture.Pixels =
        load(NewTexture.FileName, NewTexture.Size, NewTexture.Channels);

    ImageInputChunk ImageInputChunkInstance{
        .LogicalDevice = LogicalDevice,
        .PhysicalDevice = PhysicalDevice,
        .Size = NewTexture.Size,
        .Tiling = vk::ImageTiling::eOptimal,
        .Usage = vk::ImageUsageFlagBits::eTransferDst |
                 vk::ImageUsageFlagBits::eSampled,
        .MemoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
        .Format = vk::Format::eR8G8B8A8Unorm,
    };

    NewTexture.Image = VulkanTexture::createImage(ImageInputChunkInstance);

    NewTexture.ImageMemory =
        createImageMemory(ImageInputChunkInstance, NewTexture.Image);
    populate(NewTexture.Size, NewTexture.Channels, NewTexture.Pixels,
             NewTexture.Image);
    free(NewTexture.Pixels);

    NewTexture.ImageView = makeView(NewTexture.Image);
    NewTexture.Sampler = createSampler();

    Textures.push_back(NewTexture);
  }

  createDescriptorSet();
}

vk_core::VulkanTexture::~VulkanTexture() {
  for (auto Texture : Textures) {
    LogicalDevice.freeMemory(Texture.ImageMemory);
    LogicalDevice.destroyImage(Texture.Image);
    LogicalDevice.destroyImageView(Texture.ImageView);
    LogicalDevice.destroySampler(Texture.Sampler);
  }
}

void vk_core::VulkanTexture::use(vk::CommandBuffer CommandBuffer,
                                 vk::PipelineLayout PipelineLayout) {
  CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                   PipelineLayout, 1, DescriptorSet,nullptr);
}

stbi_uc *vk_core::VulkanTexture::load(const std::string &FileName,
                                      glm::vec<2, int> &Size, int &Channels) {
  stbi_uc *Pixels =
      stbi_load(FileName.c_str(), &Size.x, &Size.y, &Channels, STBI_rgb_alpha);

  if (Pixels == nullptr) {
    throw std::runtime_error("Failed to load texture image!");
  }

  return Pixels;
}

void vk_core::VulkanTexture::populate(const glm::vec<2, int> &Size,
                                      int Channels, stbi_uc *Pixels,
                                      vk::Image Image) {
  BufferInput Input{
      .Size = static_cast<uint32_t>(Size.x * Size.y * 4),
      .Usage = vk::BufferUsageFlagBits::eTransferSrc,
      .Properties = vk::MemoryPropertyFlagBits::eHostVisible |
                    vk::MemoryPropertyFlagBits::eHostCoherent,
  };
  vk::BufferCreateInfo BufferInfo{
      .flags = vk::BufferCreateFlags(),
      .size = Input.Size,
      .usage = Input.Usage,
      .sharingMode = vk::SharingMode::eExclusive,
  };

  vk::Buffer StagingBuffer;

  try {
    StagingBuffer = LogicalDevice.createBuffer(BufferInfo);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to create buffer ") +
                             E.what());
  }

  vk::MemoryRequirements MemoryRequirements =
      LogicalDevice.getBufferMemoryRequirements(StagingBuffer);

  vk::MemoryAllocateInfo AllocateInfo{
      .allocationSize = MemoryRequirements.size,
      .memoryTypeIndex = 0,
  };

  vk::PhysicalDeviceMemoryProperties MemoryProperties =
      PhysicalDevice.getMemoryProperties();

  const auto Properties = vk::MemoryPropertyFlagBits::eHostVisible |
                          vk::MemoryPropertyFlagBits::eHostCoherent;
  for (uint32_t I = 0; I < MemoryProperties.memoryTypeCount; ++I) {
    const auto Supported = MemoryRequirements.memoryTypeBits & (1 << I);
    const auto Sufficient = (MemoryProperties.memoryTypes[I].propertyFlags &
                             Properties) == Properties;
    if (Supported && Sufficient) {
      AllocateInfo.memoryTypeIndex = I;
      break;
    }
  }

  vk::DeviceMemory StagingMemory = LogicalDevice.allocateMemory(AllocateInfo);
  LogicalDevice.bindBufferMemory(StagingBuffer, StagingMemory, 0);

  void *WriteLocation = LogicalDevice.mapMemory(StagingMemory, 0, Input.Size);
  std::memcpy(WriteLocation, Pixels, Input.Size);
  LogicalDevice.unmapMemory(StagingMemory);

  ImageTransitionJob Job{
      .CommandBuffer = CommandBuffer,
      .Queue = Queue,
      .Image = Image,
      .OldImageLayout = vk::ImageLayout::eUndefined,
      .NewImageLayout = vk::ImageLayout::eTransferDstOptimal,
  };

  transitionLayout(Job);

  BufferImageCopyJob CopyJob{
      .CommandBuffer = CommandBuffer,
      .Queue = Queue,
      .SrcBuffer = StagingBuffer,
      .DstImage = Image,
      .Size = Size,
  };

  copyBufferToImage(CopyJob);

  Job.OldImageLayout = vk::ImageLayout::eTransferDstOptimal;
  Job.NewImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

  transitionLayout(Job);

  LogicalDevice.freeMemory(StagingMemory);
  LogicalDevice.destroyBuffer(StagingBuffer);
}

vk::ImageView vk_core::VulkanTexture::makeView(vk::Image Image) {
  vk::ImageViewCreateInfo ViewCreateInfo{
      .image = Image,
      .viewType = vk::ImageViewType::e2D,
      .format = vk::Format::eR8G8B8A8Unorm,
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
    return LogicalDevice.createImageView(ViewCreateInfo);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to create image view ") +
                             E.what());
  }
}

vk::Sampler vk_core::VulkanTexture::createSampler() {
  vk::SamplerCreateInfo SamplerCreateInfo{
      .magFilter = vk::Filter::eNearest,
      .minFilter = vk::Filter::eLinear,
      .mipmapMode = vk::SamplerMipmapMode::eLinear,
      .addressModeU = vk::SamplerAddressMode::eRepeat,
      .addressModeV = vk::SamplerAddressMode::eRepeat,
      .addressModeW = vk::SamplerAddressMode::eRepeat,
      .mipLodBias = 0.0f,
      .anisotropyEnable = VK_FALSE,
      .maxAnisotropy = 16.0f,
      .compareEnable = VK_FALSE,
      .compareOp = vk::CompareOp::eAlways,
      .minLod = 0.0f,
      .maxLod = 0.0f,
      .borderColor = vk::BorderColor::eIntOpaqueBlack,
      .unnormalizedCoordinates = VK_FALSE,
  };

  try {
    return LogicalDevice.createSampler(SamplerCreateInfo);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to create sampler ") +
                             E.what());
  }
}

void vk_core::VulkanTexture::createDescriptorSet() {
  vk::DescriptorSetAllocateInfo AllocateInfo{
      .descriptorPool = DescriptorPool,
      .descriptorSetCount = 1,
      .pSetLayouts = &DescriptorSetLayout,
  };

  try {
    DescriptorSet =
        LogicalDevice.allocateDescriptorSets(AllocateInfo)[0];
  } catch (vk::SystemError &E) {
    throw std::runtime_error(
        std::string("Failed to allocate descriptor set ") + E.what());
  }

  std::vector<vk::WriteDescriptorSet> WriteDescriptorSets;
  std::vector<vk::DescriptorImageInfo*> ImageInfos;
  uint32_t Index = 0;
  for (auto &Texture : Textures) {
    vk::DescriptorImageInfo *ImageInfo = new vk::DescriptorImageInfo{
        .sampler = Texture.Sampler,
        .imageView = Texture.ImageView,
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
    };
    ImageInfos.push_back(ImageInfo);

    vk::WriteDescriptorSet WriteDescriptorSet{
        .dstSet = DescriptorSet,
        .dstBinding = Index++,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        .pImageInfo = ImageInfos.back(),
    };
    WriteDescriptorSets.push_back(std::move(WriteDescriptorSet));
  }

  LogicalDevice.updateDescriptorSets(WriteDescriptorSets.size(),
                                     WriteDescriptorSets.data(), 0, nullptr);

  for (auto *ImageInfo : ImageInfos) {
    delete ImageInfo;
  }
}

vk::Image vk_core::VulkanTexture::createImage(
    vk_core::VulkanTexture::ImageInputChunk InputChunk) {
  vk::ImageCreateInfo ImageCreateInfo{
      .flags = vk::ImageCreateFlags(),
      .imageType = vk::ImageType::e2D,
      .format = InputChunk.Format,
      .extent = {(uint32_t)InputChunk.Size.x, (uint32_t)InputChunk.Size.y, 1},
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = vk::SampleCountFlagBits::e1,
      .tiling = InputChunk.Tiling,
      .usage = InputChunk.Usage,
      .sharingMode = vk::SharingMode::eExclusive,
      .initialLayout = vk::ImageLayout::eUndefined,
  };

  try {
    return InputChunk.LogicalDevice.createImage(ImageCreateInfo);
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to create image ") + E.what());
  }
}

vk::DeviceMemory vk_core::VulkanTexture::createImageMemory(
    vk_core::VulkanTexture::ImageInputChunk InputChunk, vk::Image Image) {
  vk::MemoryRequirements MemoryRequirements =
      InputChunk.LogicalDevice.getImageMemoryRequirements(Image);

  vk::MemoryAllocateInfo AllocateInfo{
      .allocationSize = MemoryRequirements.size,
      .memoryTypeIndex = 0,
  };

  vk::PhysicalDeviceMemoryProperties MemoryProperties =
      InputChunk.PhysicalDevice.getMemoryProperties();

  for (uint32_t I = 0; I < MemoryProperties.memoryTypeCount; ++I) {
    const auto Supported = MemoryRequirements.memoryTypeBits & (1 << I);
    const auto Sufficient =
        (MemoryProperties.memoryTypes[I].propertyFlags &
         InputChunk.MemoryProperties) == InputChunk.MemoryProperties;
    if (Supported && Sufficient) {
      AllocateInfo.memoryTypeIndex = I;
      break;
    }
  }

  try {
    vk::DeviceMemory ImageMemory =
        InputChunk.LogicalDevice.allocateMemory(AllocateInfo);
    InputChunk.LogicalDevice.bindImageMemory(Image, ImageMemory, 0);
    return ImageMemory;
  } catch (vk::SystemError &E) {
    throw std::runtime_error(std::string("Failed to allocate image memory ") +
                             E.what());
  }
}

void vk_core::VulkanTexture::transitionLayout(
    vk_core::VulkanTexture::ImageTransitionJob Job) {
  Job.CommandBuffer.reset();

  vk::CommandBufferBeginInfo BeginInfo{
      .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
  };
  Job.CommandBuffer.begin(BeginInfo);

  vk::ImageSubresourceRange Access{
      .aspectMask = vk::ImageAspectFlagBits::eColor,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
  };

  vk::ImageMemoryBarrier Barries{
      .srcAccessMask = vk::AccessFlags(),
      .dstAccessMask = vk::AccessFlags(),
      .oldLayout = Job.OldImageLayout,
      .newLayout = Job.NewImageLayout,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = Job.Image,
      .subresourceRange = Access,
  };

  vk::PipelineStageFlags SourceStage, DstStage;
  if (Job.OldImageLayout == vk::ImageLayout::eUndefined) {
    Barries.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
    Barries.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

    SourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
    DstStage = vk::PipelineStageFlagBits::eTransfer;
  } else if (Job.OldImageLayout == vk::ImageLayout::eTransferDstOptimal &&
             Job.NewImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
    Barries.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    Barries.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    SourceStage = vk::PipelineStageFlagBits::eTransfer;
    DstStage = vk::PipelineStageFlagBits::eFragmentShader;
  } else {
    throw std::invalid_argument("Unsupported layout transition!");
  }

  Job.CommandBuffer.pipelineBarrier(
      SourceStage, DstStage, vk::DependencyFlags(), nullptr, nullptr, Barries);

  Job.CommandBuffer.end();

  vk::SubmitInfo SubmitInfo{
      .commandBufferCount = 1,
      .pCommandBuffers = &Job.CommandBuffer,
  };

  const auto &Discard = Job.Queue.submit(1, &SubmitInfo, nullptr);
  Job.Queue.waitIdle();
}

void vk_core::VulkanTexture::copyBufferToImage(
    vk_core::VulkanTexture::BufferImageCopyJob Job) {
  Job.CommandBuffer.reset();

  vk::CommandBufferBeginInfo BeginInfo{
      .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
  };
  Job.CommandBuffer.begin(BeginInfo);

  vk::ImageSubresourceLayers Access{
      .aspectMask = vk::ImageAspectFlagBits::eColor,
      .mipLevel = 0,
      .baseArrayLayer = 0,
      .layerCount = 1,
  };
  vk::BufferImageCopy Copy{
      .bufferOffset = 0,
      .bufferRowLength = 0,
      .bufferImageHeight = 0,
      .imageSubresource = Access,
      .imageOffset = {0, 0, 0},
      .imageExtent = {static_cast<uint32_t>(Job.Size.x),
                      static_cast<uint32_t>(Job.Size.y), 1},
  };

  Job.CommandBuffer.copyBufferToImage(
      Job.SrcBuffer, Job.DstImage, vk::ImageLayout::eTransferDstOptimal, Copy);

  Job.CommandBuffer.end();

  vk::SubmitInfo SubmitInfo{
      .commandBufferCount = 1,
      .pCommandBuffers = &Job.CommandBuffer,
  };

  const auto &Discard = Job.Queue.submit(1, &SubmitInfo, nullptr);
  Job.Queue.waitIdle();
}

vk::ImageView
vk_core::VulkanTexture::createImageView(vk::Device LogicalDevice,
                                        vk::Image Image, vk::Format Format,
                                        vk::ImageAspectFlags AspectFlags) {
  vk::ImageViewCreateInfo ImageViewCreateInfo{
      .image = Image,
      .viewType = vk::ImageViewType::e2D,
      .format = Format,
      .components =
          {
              .r = vk::ComponentSwizzle::eIdentity,
              .g = vk::ComponentSwizzle::eIdentity,
              .b = vk::ComponentSwizzle::eIdentity,
              .a = vk::ComponentSwizzle::eIdentity,
          },
      .subresourceRange =
          {
              .aspectMask = AspectFlags,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
  };

  return LogicalDevice.createImageView(ImageViewCreateInfo);
}
