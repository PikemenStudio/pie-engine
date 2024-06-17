//
// Created by FullHat on 06/05/2024.
//

#ifndef ENGINE_SRC_MODULES_GRAPHICS_SRC_VULKANPIPELINE_H
#define ENGINE_SRC_MODULES_GRAPHICS_SRC_VULKANPIPELINE_H

#include "../../shader_loader/facades/facade.hpp"
#include "../../windows/facades/facade.hpp"
#include "ObjectsMemory.hpp"
#include "VulkanInstance.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanTexture.hpp"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan.hpp"
#include </Users/fullhat/Documents/GitHub/pie-engine/vcpkg/buildtrees/imgui/src/76b6bb5605-ca10468178.clean/backends/imgui_impl_glfw.h>
#include </Users/fullhat/Documents/GitHub/pie-engine/vcpkg/buildtrees/imgui/src/76b6bb5605-ca10468178.clean/backends/imgui_impl_vulkan.h>
#include <map>
#include <vector>

namespace vk_core {

#define VULKAN_PIPELINE_TEMPLATES                                              \
  template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,      \
            SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>  \
                SceneManagerImplT>

VULKAN_PIPELINE_TEMPLATES
class VulkanPhysicalDevice;

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

VULKAN_PIPELINE_TEMPLATES
class VulkanPipeline {
public:
  struct FacadesStruct {
    std::shared_ptr<WindowApiFacade<WindowImpl>> Window;
    std::shared_ptr<ShaderLoaderFacade<ShaderLoaderImplT>> ShaderLoader;
    std::shared_ptr<SceneManagerFacade<
        scene_manager_facades::SceneManagerDependencies, SceneManagerImplT>>
        SceneManager;
  };

  struct VkPipelineProps;

  VulkanPipeline(VkPipelineProps Props);
  ~VulkanPipeline();

  struct VkPipelineProps {
    std::shared_ptr<typename vk_core::VulkanPhysicalDevice<
        WindowImpl, ShaderLoaderImplT, SceneManagerImplT>>
        PhysicalDevice;
    std::shared_ptr<vk_core::VulkanInstance> Instance;
    FacadesStruct Facades;
    std::map<vk::QueueFlagBits, uint32_t> FamilyIndexes;

    std::shared_ptr<vk::Queue> GraphicsQueue;
    std::shared_ptr<vk::Queue> PresentQueue;
  };

  struct PublicObjectData {
    std::vector<float> Vertices;
    std::vector<float> Colors;
    std::vector<float> TexCoords;
    std::vector<uint32_t> Indexes;
    std::vector<float> Normals;
    std::string ObjectName;
  };

  using PublicObjectDataMap = std::map<std::string, PublicObjectData>;

  void addObjectData(
      const std::map<std::string, VulkanPipeline::PublicObjectData> &Dump,
      const std::string &DumpName);

  using TextureSet = std::pair<std::string, std::vector<std::string>>;
  void addTextureSet(TextureSet &&TextureSet);

  void addShaderSet(const std::string &VertexPath,
                    const std::string &FragmentPath, const std::string &Name);

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
    vk::Image DepthBuffer;
    vk::DeviceMemory DepthBufferMemory;
    vk::ImageView DepthBufferView;
    vk::Format DepthFormat;
    vk::Semaphore ImageAvailableSemaphore = nullptr;
    vk::Semaphore RenderFinishedSemaphore = nullptr;
    vk::Fence InFlightFence = nullptr;
    vk::Buffer CameraBuffer;
    vk::DeviceMemory CameraMemory;
    void *CameraDataWriteLocation;
    vk::DescriptorBufferInfo UniformBufferDescriptor;
    vk::DescriptorBufferInfo ModelBufferDescriptor;
    vk::DescriptorSet DescriptorSet;
    std::vector<glm::mat4> ModelTransforms;
    vk::Buffer ModelBuffer;
    vk::DeviceMemory ModelMemory;
    void *ModelDataWriteLocation;
    vk::Buffer LightBuffer;
    vk::DeviceMemory LightMemory;
    void *LightDataWriteLocation;
    vk::DescriptorBufferInfo LightBufferDescriptor;
  };

  struct SwapChainBundleStruct {
    vk::SwapchainKHR Swapchain;
    std::vector<SwapChainFrameStruct> Frames;
    vk::Format Format, DepthFormat;
    vk::Extent2D Extent;
  };
  std::optional<SwapChainBundleStruct> SwapChainBundle;

  struct NativeComponentsStruct {
    std::shared_ptr<typename vk_core::VulkanPhysicalDevice<
        WindowImpl, ShaderLoaderImplT, SceneManagerImplT>>
        PhysicalDevice;
    std::shared_ptr<vk_core::VulkanInstance> Instance;

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
  // map to use several shader sets
  std::map<std::string, GraphicsPipelineOutBundle> PipelineBundles;

  vk::CommandPool CommandPool = nullptr;
  vk::CommandBuffer MainCommandBuffer = nullptr;

  int MaxFrameInFlight, FrameNumber = 0;

  struct MeshData {
    std::vector<float> Data;
    std::vector<uint32_t> Indexes;
    int Offset;
    int Size;
    uint32_t IndexCount;
    uint32_t FirstIndex;
  };

  std::unordered_map<std::string, VulkanTexture *> Textures;

  struct BufferInput {
    vk::DeviceSize Size;
    vk::BufferUsageFlags Usage;
    vk::MemoryPropertyFlags Properties;
  };

  struct MeshDump {
    vk::Buffer Buffer;
    vk::DeviceMemory Memory;
    vk::Buffer IndexBuffer;
    vk::DeviceMemory IndexMemory;
    std::map<std::string, MeshData> Meshes;
    std::vector<float> DataCache;
    std::vector<uint32_t> IndexLump;
    std::vector<uint32_t> FirstIndexes;

    void recalculateDataCache() {
      int IndexOffset = 0;
      DataCache.clear();
      for (auto &[Name, Mesh] : Meshes) {
        int VertexCount = Mesh.Data.size() / 7;
        int IndexCount = Mesh.Indexes.size();
        int LastIndex = IndexLump.size();

        for (const auto &Data : Mesh.Data) {
          DataCache.push_back(Data);
        }

        for (const auto &Index : Mesh.Indexes) {
          IndexLump.push_back(Index + IndexOffset);
        }
        IndexOffset += VertexCount;

        FirstIndexes.push_back(LastIndex);
      }
    };
  };

  struct MeshTypes {
    std::map<std::string, MeshDump> Dumps;
  } MeshTypes;

  vk::DescriptorSetLayout FrameSetLayout = nullptr;
  vk::DescriptorPool FramePool = nullptr;
  vk::DescriptorSetLayout MeshSetLayout = nullptr;
  vk::DescriptorPool MeshPool = nullptr;

  struct DescriptorSetLayoutInputStruct {
    int Count;
    std::vector<int> Indexes;
    std::vector<vk::DescriptorType> Types;
    std::vector<int> Counts;
    std::vector<vk::ShaderStageFlags> Stages;
  };

  constexpr static size_t MaxModelsPerRender = 1000;
  constexpr static size_t MaxUniformPointLights = 10;

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

  void createDescriptorSetLayouts(DescriptorSetLayoutInputStruct Input);
  vk::DescriptorPool
  createDescriptorPool(uint32_t Size, DescriptorSetLayoutInputStruct &Input);
  void writeDescriptorSet(SwapChainFrameStruct &Frame);
  GraphicsPipelineOutBundle
  createPipeline(GraphicsPipelineInBundle InBundle) const;

  void createFrameBuffers();
  void createCommandPool();
  void createMainCommandBuffer();
  void createFrameCommandBuffers();

  void createSemaphores();
  void createFences();

  vk::VertexInputBindingDescription getBindingDescription() const;
  std::array<vk::VertexInputAttributeDescription, 4>
  getAttributeDescriptions() const;

  void recordDrawCommands(vk::CommandBuffer CommandBuffer, uint32_t ImageIndex);
  size_t drawObjects(SceneManagerFacadeStructs::OneTypeObjects Objects,
                   vk::CommandBuffer CommandBuffer, uint32_t Offset,
                   std::string ShaderSetName);

  void createBuffer(BufferInput Input, MeshDump &Buffer);
  void copyBuffer(vk::Buffer SrcBuffer, vk::Buffer DstBuffer,
                  vk::DeviceSize Size, vk::Queue Queue,
                  vk::CommandBuffer CommandBuffer);
  uint32_t findMemoryType(uint32_t TypeFilter,
                          vk::MemoryPropertyFlags Properties);
  void allocateBufferMemory(MeshDump &Mesh);
  void allocateDescriptorSet(SwapChainFrameStruct &Frame);

  void prepareScene(vk::CommandBuffer CommandBuffer, MeshDump &DumpIndex);
  void prepareFrame(uint32_t ImageIndex);

  void createDescriptorBuffer(SwapChainFrameStruct &Frame);
  void initUi();

  vk::Format findSupportedFormat(const std::vector<vk::Format> &Candidates,
                                 vk::ImageTiling Tiling,
                                 vk::FormatFeatureFlags Features);

  void CreateDepthResources(SwapChainFrameStruct &Frame);
};

} // namespace vk_core

#endif // ENGINE_SRC_MODULES_GRAPHICS_SRC_VULKANPIPELINE_H
