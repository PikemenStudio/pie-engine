//
// Created by FullHat on 03/04/2024.
//

#ifndef ENGINE_SRC_MODULES_VK_CORE_SRC_GRAPHICENGINE_HPP
#define ENGINE_SRC_MODULES_VK_CORE_SRC_GRAPHICENGINE_HPP

#include "../../scene_manager/facades/facade.hpp"
#include "../../shader_loader/facades/facade.hpp"
#include "../../windows/facades/facade.hpp"
#include "VulkanInstance.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanPipeline.h"
#include "loguru/loguru.hpp"

namespace vk_core {

template <
    WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderImpl,
    SceneManagerImpl<scene_manager_facades::SceneManagerDependencies> SceneImpl>
class GraphicEngine {
  using PhysicalDeviceType =
      VulkanPhysicalDevice<WindowImpl, ShaderImpl, SceneImpl>;

public:
  struct GraphicEngineProps;

  GraphicEngine(GraphicEngineProps Props);
  ~GraphicEngine();

  struct GraphicEngineProps {
    std::shared_ptr<WindowApiFacade<WindowImpl>> Window;
    std::shared_ptr<ShaderLoaderFacade<ShaderImpl>> ShaderLoader;
    std::shared_ptr<SceneManagerFacade<
        scene_manager_facades::SceneManagerDependencies, SceneImpl>>
        SceneManager;

    vk_core::VulkanInstance::VkInstanceProps VkInstanceProps;
    vk_core::VulkanPhysicalDevice<WindowImpl, ShaderImpl,
                                  SceneImpl>::VkPhysicalDeviceProps
        VkPhysicalDeviceProps;
  };

  void setupInstance(VulkanInstance::VkInstanceProps Props);
  void setupPhysicalDevice(
      VulkanPhysicalDevice<WindowImpl, ShaderImpl,
                           SceneImpl>::VkPhysicalDeviceProps Props);

  std::vector<typename VulkanPhysicalDevice<
      WindowImpl, ShaderImpl, SceneImpl>::PhysicalDeviceLocalProps>
  getLocalPhysicalDevices() const;

  void chooseLocalPhysicalDevice(
      const VulkanPhysicalDevice<WindowImpl, ShaderImpl,
                                 SceneImpl>::PhysicalDeviceLocalProps &Device);

  enum class DeviceChoosePolicy : uint_fast8_t {
    FIRST,
    BEST,
  };
  // Call with default choose policy
  // If ChooseAnyWayIfFailed is true, then the function will try to choose any
  // in the BEST policy ONLY
  void chooseLocalPhysicalDevice(const DeviceChoosePolicy Policy);

  void
  addObject(const std::map<std::string,
                           typename VulkanPipeline<WindowImpl, ShaderImpl,
                                                   SceneImpl>::PublicObjectData>
                &Dump,
            const std::string &DumpName);
  void addTextureSet(PhysicalDeviceType::PipelineType::TextureSet &&TextureSet);

  void addShaderSet(const std::string &VertexPath,
                    const std::string &FragmentPath, const std::string &Name);

  void render();

private:
  struct AdaptersStruct {
    std::shared_ptr<WindowApiFacade<WindowImpl>> Window;
    std::shared_ptr<ShaderLoaderFacade<ShaderImpl>> ShaderLoader;
    std::shared_ptr<SceneManagerFacade<
        scene_manager_facades::SceneManagerDependencies, SceneImpl>>
        SceneManager;
  };

  struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR Capabilities;
    std::vector<vk::SurfaceFormatKHR> Formats;
    std::vector<vk::PresentModeKHR> PresentModes;
  };

  struct NativeComponentsStruct {
    std::optional<AdaptersStruct> Adapters;

    std::shared_ptr<vk_core::VulkanInstance> Instance;
    std::shared_ptr<
        vk_core::VulkanPhysicalDevice<WindowImpl, ShaderImpl, SceneImpl>>
        PhysicalDevice;
  } NativeComponents;
};

} // namespace vk_core

#endif // ENGINE_SRC_MODULES_VK_CORE_SRC_GRAPHICENGINE_HPP
