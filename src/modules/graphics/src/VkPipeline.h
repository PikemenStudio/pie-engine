//
// Created by FullHat on 06/05/2024.
//

#ifndef ENGINE_SRC_MODULES_GRAPHICS_SRC_VKPIPELINE_H
#define ENGINE_SRC_MODULES_GRAPHICS_SRC_VKPIPELINE_H

#include "../../shader_loader/facades/facade.hpp"
#include "../../windows/facades/facade.hpp"
#include "ObjectsMemory.hpp"
#include "VkInstance.hpp"
#include "VkPhysicalDevice.hpp"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan.hpp"
#include <map>
#include <vector>

namespace vk_core {

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
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
template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
class VkPipeline {
public:
  struct FacadesStruct {
    std::shared_ptr<WindowApiFacade<WindowImpl>> Window;
    std::shared_ptr<ShaderLoaderFacade<ShaderLoaderImplT>> ShaderLoader;
    std::shared_ptr<SceneManagerFacade<
        scene_manager_facades::SceneManagerDependencies, SceneManagerImplT>>
        SceneManager;
  };

  struct VkPipelineProps;

  VkPipeline(VkPipelineProps Props);
  ~VkPipeline();

  struct VkPipelineProps {
    std::shared_ptr<typename vk_core::VkPhysicalDevice<
        WindowImpl, ShaderLoaderImplT, SceneManagerImplT>>
        PhysicalDevice;
    std::shared_ptr<vk_core::VkInstance> Instance;
    FacadesStruct Facades;
    std::map<vk::QueueFlagBits, uint32_t> FamilyIndexes;

    std::shared_ptr<vk::Queue> GraphicsQueue;
    std::shared_ptr<vk::Queue> PresentQueue;
  };

  struct PublicObjectData {
    std::vector<float> Vertices;
    std::vector<float> Colors;
    std::string ObjectName;
  };

  using PublicObjectDataMap = std::map<std::string, PublicObjectData>;

  void
  addObjectData(const std::map<std::string, VkPipeline::PublicObjectData> &Dump,
                const std::string &DumpName);

  void render();

protected:
  struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR Capabilities;
    std::vector<vk::SurfaceFormatKHR> Formats;
    std::vector<vk::PresentModeKHR> PresentModes;
  };

  struct SwapChainFrameStruct {
    vk::Image Image;
    vk::ImageView ImageView;
    vk::Framebuffer FrameBuffer;
    vk::CommandBuffer CommandBuffer;
    vk::Semaphore ImageAvailableSemaphore = nullptr;
    vk::Semaphore RenderFinishedSemaphore = nullptr;
    vk::Fence InFlightFence = nullptr;
  };

  struct SwapChainBundleStruct {
    vk::SwapchainKHR Swapchain;
    std::vector<SwapChainFrameStruct> Frames;
    vk::Format Format;
    vk::Extent2D Extent;
  };
  std::optional<SwapChainBundleStruct> SwapChainBundle;

  struct NativeComponentsStruct {
    std::shared_ptr<typename vk_core::VkPhysicalDevice<
        WindowImpl, ShaderLoaderImplT, SceneManagerImplT>>
        PhysicalDevice;
    std::shared_ptr<vk_core::VkInstance> Instance;

    std::optional<FacadesStruct> Facades;
    std::map<vk::QueueFlagBits, uint32_t> FamilyIndexes;

    std::optional<vk::SurfaceKHR> Surface;

    std::optional<SwapChainSupportDetails> SwapChainSupport;

    std::shared_ptr<vk::Queue> GraphicsQueue;
    std::shared_ptr<vk::Queue> PresentQueue;

  } NativeComponents;

  struct GraphicsPipelineInBundle {
    std::string VertexShaderPath;
    std::string FragmentShaderPath;
  };

  struct GraphicsPipelineOutBundle {
    vk::Pipeline Pipeline;
    vk::PipelineLayout Layout;
    vk::RenderPass RenderPass;
  };
  std::optional<GraphicsPipelineOutBundle> PipelineBundle;

  vk::CommandPool CommandPool = nullptr;
  vk::CommandBuffer MainCommandBuffer = nullptr;

  int MaxFrameInFlight, FrameNumber = 0;

  struct MeshData {
    std::vector<float> Data;
    int Offset;
    int Size;
  };

  struct BufferInput {
    vk::DeviceSize Size;
    vk::BufferUsageFlags Usage;
    vk::MemoryPropertyFlags Properties;
  };

  struct MeshDump {
    vk::Buffer Buffer;
    vk::DeviceMemory Memory;
    std::map<std::string, MeshData> Meshes;
    std::vector<float> DataCache;

    void recalculateDataCache() {
      DataCache.clear();
      for (auto &[Name, Mesh] : Meshes) {
        DataCache.resize(DataCache.size() + Mesh.Data.size());
        std::copy(Mesh.Data.begin(), Mesh.Data.end(),
                  DataCache.end() - Mesh.Data.size());
      }
    };
  };

  struct MeshTypes {
    std::map<std::string, MeshDump> Dumps;
  } MeshTypes;

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
  void destroySwapChain();
  void recreateSwapChain();

  vk::ShaderModule createShaderModule(std::string ShaderPath) const;
  vk::PipelineLayout createPipelineLayout() const;
  vk::RenderPass createRenderPass() const;

  void createPipeline(GraphicsPipelineInBundle InBundle);

  void createFrameBuffers();
  void createCommandPool();
  void createMainCommandBuffer();
  void createFrameCommandBuffers();

  void createSemaphores();
  void createFences();

  vk::VertexInputBindingDescription getBindingDescription();
  std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions();

  void recordDrawCommands(vk::CommandBuffer CommandBuffer, uint32_t ImageIndex);

  void createBuffer(BufferInput Input, MeshDump &Buffer);
  uint32_t findMemoryType(uint32_t TypeFilter,
                          vk::MemoryPropertyFlags Properties);
  void allocateBufferMemory(MeshDump &Mesh);

  void prepareScene(vk::CommandBuffer CommandBuffer, MeshDump &DumpIndex);
};

} // namespace vk_core

#endif // ENGINE_SRC_MODULES_GRAPHICS_SRC_VKPIPELINE_H
