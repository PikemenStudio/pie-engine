//
// Created by FullHat on 19/05/2024.
//

#ifndef ENGINE_SRC_MODULES_GRAPHICS_SRC_VULKANTEXTURE_HPP
#define ENGINE_SRC_MODULES_GRAPHICS_SRC_VULKANTEXTURE_HPP

#include "glm/glm.hpp"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "stb_image.h"
#include "vulkan/vulkan.hpp"

namespace vk_core {

class VulkanTexture {
public:
  struct TextureInputChunk {
    vk::Device LogicalDevice;
    vk::PhysicalDevice PhysicalDevice;
    vk::CommandBuffer CommandBuffer;
    vk::Queue Queue;
    vk::DescriptorSetLayout DescriptorSetLayout;
    vk::DescriptorPool DescriptorPool;
    std::vector<std::string> FileNames;
  };

  struct ImageInputChunk {
    vk::Device LogicalDevice;
    vk::PhysicalDevice PhysicalDevice;
    glm::vec<2, int> Size;
    vk::ImageTiling Tiling;
    vk::ImageUsageFlags Usage;
    vk::MemoryPropertyFlags MemoryProperties;
    vk::Format Format;
  };

  struct ImageTransitionJob {
    vk::CommandBuffer CommandBuffer;
    vk::Queue Queue;
    vk::Image Image;
    vk::ImageLayout OldImageLayout, NewImageLayout;
  };

  struct BufferImageCopyJob {
    vk::CommandBuffer CommandBuffer;
    vk::Queue Queue;
    vk::Buffer SrcBuffer;
    vk::Image DstImage;
    glm::vec<2, int> Size;
  };

  VulkanTexture(TextureInputChunk InputChunk);
  ~VulkanTexture();

  void use(vk::CommandBuffer CommandBuffer, vk::PipelineLayout PipelineLayout);

  static stbi_uc *load(const std::string &FileName, glm::vec<2, int> &Size,
                       int &Channels);
  void populate(const glm::vec<2, int> &Size, int Channels, stbi_uc *Pixels,
                vk::Image Image);
  vk::ImageView makeView(vk::Image Image);
  vk::Sampler createSampler();
  void createDescriptorSet();

public:
  static vk::Image createImage(ImageInputChunk InputChunk);
  static vk::DeviceMemory createImageMemory(ImageInputChunk InputChunk,
                                            vk::Image Image);
  static void transitionLayout(ImageTransitionJob Job);
  static void copyBufferToImage(BufferImageCopyJob Job);
  static vk::ImageView createImageView(vk::Device LogicalDevice,
                                       vk::Image Image, vk::Format Format,
                                       vk::ImageAspectFlags AspectFlags);

protected:
  struct BufferInput {
    vk::DeviceSize Size;
    vk::BufferUsageFlags Usage;
    vk::MemoryPropertyFlags Properties;
  };

  struct TextureData {
    glm::vec<2, int> Size;
    int Channels;

    std::string FileName;

    stbi_uc *Pixels;

    vk::Image Image;
    vk::DeviceMemory ImageMemory;
    vk::ImageView ImageView;
    vk::Sampler Sampler;
  };

  std::vector<TextureData> Textures;

  vk::Device LogicalDevice;
  vk::PhysicalDevice PhysicalDevice;

  vk::DescriptorSetLayout DescriptorSetLayout;
  vk::DescriptorPool DescriptorPool;

  vk::CommandBuffer CommandBuffer;
  vk::Queue Queue;

  vk::DescriptorSet DescriptorSet;
};

} // namespace vk_core

#endif // ENGINE_SRC_MODULES_GRAPHICS_SRC_VULKANTEXTURE_HPP
