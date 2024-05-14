//
// Created by FullHat on 03/04/2024.
//

#ifndef ENGINE_SRC_MODULES_VK_CORE_SRC_VKPHYSICALDEVICE_HPP
#define ENGINE_SRC_MODULES_VK_CORE_SRC_VKPHYSICALDEVICE_HPP

#include <memory>

#include "../../scene_manager/facades/facade.hpp"
#include "../../windows/facades/facade.hpp"
#include "VkInstance.hpp"
#include "VkPipeline.h"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan.hpp"
#include <map>

namespace vk_core {

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
class VkPipeline;

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
class VkPhysicalDevice {
public:
  struct VkPhysicalDeviceProps;

  VkPhysicalDevice(VkPhysicalDeviceProps Props);
  ~VkPhysicalDevice();

  VkPhysicalDevice(const VkPhysicalDevice &PdToCopy) = delete;
  VkPhysicalDevice(VkPhysicalDevice &&PdToMove);
  VkPhysicalDevice &operator=(const VkPhysicalDevice &PdToCopy) = delete;
  VkPhysicalDevice &operator=(VkPhysicalDevice &&PdToMove);

  struct VkPhysicalDeviceProps {
    std::shared_ptr<VkInstance> Instance;
  };

  explicit operator vk::PhysicalDevice &() { return PhysicalDevice.value(); }
  explicit operator vk::Device &() { return LogicalDevice.value(); }

public:
  [[maybe_unused]] std::vector<vk::PhysicalDevice>
  getNativePhysicalDevices() const;

public:
  struct PhysicalDeviceLocalProps {
    std::string Name;
    uint32_t ApiVersion;
    uint32_t DriverVersion;
    uint32_t VendorId;
    uint32_t DeviceId;

    enum class TypeEnum;

    bool operator==(const vk::PhysicalDevice &Device) const {
      const vk::PhysicalDeviceProperties Properties = Device.getProperties();
      return this->Name == Properties.deviceName &&
             this->ApiVersion == Properties.apiVersion &&
             this->DeviceId == Properties.deviceID &&
             this->DriverVersion == Properties.driverVersion &&
             this->VendorId == Properties.vendorID;
    }

    static TypeEnum toLocalDeviceType(vk::PhysicalDeviceType DeviceType) {
      switch (DeviceType) {
      case vk::PhysicalDeviceType::eOther:
        return TypeEnum::OTHER;
      case vk::PhysicalDeviceType::eIntegratedGpu:
        return TypeEnum::INTEGRATED_GPU;
      case vk::PhysicalDeviceType::eDiscreteGpu:
        return TypeEnum::DISCRETE_GPU;
      case vk::PhysicalDeviceType::eVirtualGpu:
        return TypeEnum::VIRTUAL_GPU;
      case vk::PhysicalDeviceType::eCpu:
        return TypeEnum::CPU;
      default:
        return TypeEnum::OTHER;
      }
    }

    enum class TypeEnum {
      OTHER = 0,
      INTEGRATED_GPU = 1,
      DISCRETE_GPU = 2,
      VIRTUAL_GPU = 3,
      CPU = 4,
    } Type;
  };

  std::vector<PhysicalDeviceLocalProps> getLocalPhysicalDevices() const;

  void chooseDeviceAndSetup(
      std::function<bool(const vk::PhysicalDevice &)> Predicate);

  void findQueueIndexesAndSetup();

  void setupLogicalDevice();

  void setupQueues();

  struct PipelineInitDataStruct {
    std::shared_ptr<WindowApiFacade<WindowImpl>> Window;
    std::shared_ptr<ShaderLoaderFacade<ShaderLoaderImplT>> ShaderLoader;
    std::shared_ptr<SceneManagerFacade<scene_manager_facades::SceneManagerDependencies, SceneManagerImplT>> SceneManager;
    // It's strange, but we need pointer from GraphicEngine and can't just use
    // this
    std::shared_ptr<
        VkPhysicalDevice<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>>
        ThisPhysicalDevice;
  };
  void setupPipeline(const PipelineInitDataStruct &&PipelineInitData);

  void render();

protected:
  static void swap(VkPhysicalDevice &Pd1, VkPhysicalDevice &Pd2);

protected:
  std::shared_ptr<VkInstance> Instance;
  std::optional<vk::PhysicalDevice> PhysicalDevice;
  std::optional<vk::Device> LogicalDevice;
  std::unique_ptr<typename vk_core::VkPipeline<WindowImpl, ShaderLoaderImplT,
                                               SceneManagerImplT>>
      Pipeline;

  struct QueueIndexes {
    std::optional<uint32_t> Graphics;
    std::optional<uint32_t> Present;
    std::optional<uint32_t> Transfer;

    bool isComplete() {
      return Graphics.has_value() && Present.has_value() &&
             Transfer.has_value();
    }
  } QueueIndexesInstance;

  std::shared_ptr<vk::Queue> GraphicsQueue;
  std::shared_ptr<vk::Queue> PresentQueue;

  static inline constinit bool Debug = true;
};

} // namespace vk_core

#endif // ENGINE_SRC_MODULES_VK_CORE_SRC_VKPHYSICALDEVICE_HPP
