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
    std::string FileName;
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

  void load();
  void populate();
  void makeView();
  void createSampler();
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

  glm::vec<2, int> Size;
  int Channels;
  vk::Device LogicalDevice;
  vk::PhysicalDevice PhysicalDevice;
  vk::Image Image;
  vk::ImageView ImageView;
  vk::DeviceMemory ImageMemory;
  vk::Sampler Sampler;

  vk::DescriptorSetLayout DescriptorSetLayout;
  vk::DescriptorSet DescriptorSet;
  vk::DescriptorPool DescriptorPool;

  vk::CommandBuffer CommandBuffer;
  vk::Queue Queue;

  std::string FileName;

  stbi_uc *Pixels;
};

} // namespace vk_core

#endif // ENGINE_SRC_MODULES_GRAPHICS_SRC_VULKANTEXTURE_HPP
