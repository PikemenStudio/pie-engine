//
// Created by FullHat on 03/04/2024.
//

#include "GraphicEngine.hpp"
#include "loguru/loguru.hpp"

// Include only if glfw implementation required
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vk_core {
template <
    WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderImpl,
    SceneManagerImpl<scene_manager_facades::SceneManagerDependencies> SceneImpl>
void GraphicEngine<WindowImpl, ShaderImpl, SceneImpl>::setupInstance(
    VkInstance::VkInstanceProps Props) {
  LOG_F(INFO, "Setting up instance");
  NativeComponents.Instance = std::make_shared<vk_core::VkInstance>(Props);
  LOG_F(INFO, "Instance set up");
}

template <
    WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderImpl,
    SceneManagerImpl<scene_manager_facades::SceneManagerDependencies> SceneImpl>
void GraphicEngine<WindowImpl, ShaderImpl, SceneImpl>::setupPhysicalDevice(
    typename VkPhysicalDevice<WindowImpl, ShaderImpl,
                              SceneImpl>::VkPhysicalDeviceProps Props) {
  LOG_F(INFO, "Setting up physical device");
  NativeComponents.PhysicalDevice = std::make_shared<
      vk_core::VkPhysicalDevice<WindowImpl, ShaderImpl, SceneImpl>>(Props);
  LOG_F(INFO, "Physical device set up");
}

template <
    WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderImpl,
    SceneManagerImpl<scene_manager_facades::SceneManagerDependencies> SceneImpl>
GraphicEngine<WindowImpl, ShaderImpl, SceneImpl>::GraphicEngine(
    GraphicEngine::GraphicEngineProps Props) {
  LOG_F(INFO, "Creating Graphic Engine");

  NativeComponents.Adapters.emplace(
      AdaptersStruct{.Window = Props.Window,
                     .ShaderLoader = Props.ShaderLoader,
                     .SceneManager = Props.SceneManager});

  setupInstance(VkInstance::VkInstanceProps(Props.VkInstanceProps));

  if (Props.VkPhysicalDeviceProps.Instance == nullptr) {
    Props.VkPhysicalDeviceProps.Instance = NativeComponents.Instance;
  }
  setupPhysicalDevice(
      typename VkPhysicalDevice<WindowImpl, ShaderImpl, SceneImpl>::
          VkPhysicalDeviceProps(Props.VkPhysicalDeviceProps));
  LOG_F(INFO, "Graphic Engine created");
}

template <
    WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderImpl,
    SceneManagerImpl<scene_manager_facades::SceneManagerDependencies> SceneImpl>
GraphicEngine<WindowImpl, ShaderImpl, SceneImpl>::~GraphicEngine() {
  NativeComponents.PhysicalDevice.reset();
  // NativeComponents.Pipeline.reset();
  NativeComponents.Instance.reset();
  LOG_F(INFO, "Engine destroyed, ok");
}

template <
    WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderImpl,
    SceneManagerImpl<scene_manager_facades::SceneManagerDependencies> SceneImpl>
std::vector<typename VkPhysicalDevice<WindowImpl, ShaderImpl,
                                      SceneImpl>::PhysicalDeviceLocalProps>
GraphicEngine<WindowImpl, ShaderImpl, SceneImpl>::getLocalPhysicalDevices()
    const {
  if (NativeComponents.PhysicalDevice == nullptr) {
    LOG_F(INFO, "Physical device is null");
    throw std::runtime_error("Physical device is null");
  }

  return NativeComponents.PhysicalDevice->getLocalPhysicalDevices();
}

template <
    WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderImpl,
    SceneManagerImpl<scene_manager_facades::SceneManagerDependencies> SceneImpl>
void GraphicEngine<WindowImpl, ShaderImpl, SceneImpl>::
    chooseLocalPhysicalDevice(
        const typename VkPhysicalDevice<WindowImpl, ShaderImpl,
                                        SceneImpl>::PhysicalDeviceLocalProps
            &Device) {
  if (NativeComponents.PhysicalDevice == nullptr) {
    LOG_F(INFO, "Physical device is null");
    throw std::runtime_error("Physical device is null");
  }
  this->NativeComponents.PhysicalDevice->chooseDeviceAndSetup(
      [&](const vk::PhysicalDevice &ArgDevice) -> bool {
        return Device == ArgDevice;
      });

  this->NativeComponents.PhysicalDevice->findQueueIndexesAndSetup();

  this->NativeComponents.PhysicalDevice->setupLogicalDevice();
}

template <
    WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderImpl,
    SceneManagerImpl<scene_manager_facades::SceneManagerDependencies> SceneImpl>
void GraphicEngine<WindowImpl, ShaderImpl, SceneImpl>::
    chooseLocalPhysicalDevice(const DeviceChoosePolicy Policy) {
  LOG_F(INFO, "Choosing local physical device");

  if (NativeComponents.PhysicalDevice == nullptr) {
    LOG_F(INFO, "Physical device is null");
    throw std::runtime_error("Physical device is null");
  }

  switch (Policy) {
  case DeviceChoosePolicy::FIRST:
    this->NativeComponents.PhysicalDevice->chooseDeviceAndSetup(
        [](const vk::PhysicalDevice &ArgDevice) -> bool { return true; });
    break;
  case DeviceChoosePolicy::BEST:
    this->NativeComponents.PhysicalDevice->chooseDeviceAndSetup(
        [](const vk::PhysicalDevice &ArgDevice) -> bool {
          const auto Properties = ArgDevice.getProperties();
          return Properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
        });
    break;
  }

  this->NativeComponents.PhysicalDevice->findQueueIndexesAndSetup();

  this->NativeComponents.PhysicalDevice->setupLogicalDevice();

  this->NativeComponents.PhysicalDevice->setupQueues();

  this->NativeComponents.PhysicalDevice->setupPipeline({
      .Window = NativeComponents.Adapters->Window,
      .ShaderLoader = NativeComponents.Adapters->ShaderLoader,
      .SceneManager = NativeComponents.Adapters->SceneManager,
      .ThisPhysicalDevice = NativeComponents.PhysicalDevice,
  });

  LOG_F(INFO, "Local physical device chosen");
}

template <
    WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderImpl,
    SceneManagerImpl<scene_manager_facades::SceneManagerDependencies> SceneImpl>
void GraphicEngine<WindowImpl, ShaderImpl, SceneImpl>::render() {
  LOG_F(INFO, "Rendering");
  NativeComponents.PhysicalDevice->render();
  LOG_F(INFO, "Rendered");
}

template class vk_core::GraphicEngine<
    window_api_impls::WindowApiFacadeGlfwImpl,
    shader_loader_impls::ShaderLoaderSimpleImpl,
    scene_manager_facades::SceneManagerBaseImpl<
        scene_manager_facades::SceneManagerDependencies>>;

} // namespace vk_core