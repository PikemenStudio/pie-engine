//
// Created by FullHat on 03/04/2024.
//

#ifndef ENGINE_SRC_MODULES_VK_CORE_SRC_VKPHYSICALDEVICE_HPP
#define ENGINE_SRC_MODULES_VK_CORE_SRC_VKPHYSICALDEVICE_HPP

#include <memory>

#include "../../scene_manager/facades/facade.hpp"
#include "../../windows/facades/facade.hpp"
#include "VulkanInstance.hpp"
#include "VulkanPipeline.h"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan.hpp"
#include <map>

namespace vk_core {

#define VULKAN_PHYSICAL_DEVICE_TEMPLATES                                       \
  template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,      \
            SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>  \
                SceneManagerImplT>

VULKAN_PHYSICAL_DEVICE_TEMPLATES
class VulkanPipeline;

VULKAN_PHYSICAL_DEVICE_TEMPLATES
class VulkanPhysicalDevice {
public:
  struct VkPhysicalDeviceProps;

  VulkanPhysicalDevice(VkPhysicalDeviceProps Props);
  ~VulkanPhysicalDevice();

  VulkanPhysicalDevice(const VulkanPhysicalDevice &PdToCopy) = delete;
  VulkanPhysicalDevice(VulkanPhysicalDevice &&PdToMove);
  VulkanPhysicalDevice &
  operator=(const VulkanPhysicalDevice &PdToCopy) = delete;
  VulkanPhysicalDevice &operator=(VulkanPhysicalDevice &&PdToMove);

  struct VkPhysicalDeviceProps {
    std::shared_ptr<VulkanInstance> Instance;
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
    std::shared_ptr<SceneManagerFacade<
        scene_manager_facades::SceneManagerDependencies, SceneManagerImplT>>
        SceneManager;
    // It's strange, but we need pointer from GraphicEngine and can't just use
    // this
    std::shared_ptr<
        VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>>
        ThisPhysicalDevice;
  };
  void setupPipeline(const PipelineInitDataStruct &&PipelineInitData);

  void render();

  void addObjectData(
      const std::map<
          std::string,
          typename VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                                  SceneManagerImplT>::PublicObjectData> &Dump,
      const std::string &DumpName);
  void addTexture(const std::string &TexturePath,
                  const std::string &TextureName);

protected:
  static void swap(VulkanPhysicalDevice &Pd1, VulkanPhysicalDevice &Pd2);

protected:
  std::shared_ptr<VulkanInstance> Instance;
  std::optional<vk::PhysicalDevice> PhysicalDevice;
  std::optional<vk::Device> LogicalDevice;
  std::unique_ptr<typename vk_core::VulkanPipeline<
      WindowImpl, ShaderLoaderImplT, SceneManagerImplT>>
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
