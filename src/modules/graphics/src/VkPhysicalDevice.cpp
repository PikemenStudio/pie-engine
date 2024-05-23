//
// Created by FullHat on 03/04/2024.
//

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanPhysicalDevice.hpp"
#define LOGURU_WITH_STREAMS 1
#include "loguru/loguru.hpp"
#include <set>

namespace vk_core {

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    VulkanPhysicalDevice(VulkanPhysicalDevice::VkPhysicalDeviceProps Props) {
  Instance = Props.Instance;
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
std::vector<vk::PhysicalDevice>
VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT,
                 SceneManagerImplT>::getNativePhysicalDevices() const {
  if (Instance == nullptr) {
    LOG_F(ERROR, "Instance is not initialized");
    throw std::runtime_error("Instance is not initialized");
  }

  const auto Devices =
      static_cast<vk::Instance>(*Instance).enumeratePhysicalDevices();

  std::for_each(
      Devices.begin(), Devices.end(), [&](const vk::PhysicalDevice &Device) {
        const auto Properties = Device.getProperties();
        LOG_F(INFO, "Physical device properties:");
        // Print all properties of device
        LOG_F(INFO, "Physical device properties:");
        LOG_F(INFO, "Device Name: %s",
              std::string(Properties.deviceName).c_str());
        LOG_F(INFO, "Device Type: %d", static_cast<int>(Properties.deviceType));
        LOG_F(INFO, "API Version: %d.%d.%d",
              VK_VERSION_MAJOR(Properties.apiVersion),
              VK_VERSION_MINOR(Properties.apiVersion),
              VK_VERSION_PATCH(Properties.apiVersion));
        LOG_F(INFO, "Driver Version: %d.%d.%d",
              VK_VERSION_MAJOR(Properties.driverVersion),
              VK_VERSION_MINOR(Properties.driverVersion),
              VK_VERSION_PATCH(Properties.driverVersion));
        LOG_F(INFO, "Vendor ID: %d", Properties.vendorID);
        LOG_F(INFO, "Device ID: %d", Properties.deviceID);
      });
  return Devices;
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT,
                 SceneManagerImplT>::~VulkanPhysicalDevice() {
  this->Pipeline.reset();
  this->LogicalDevice->destroy();
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::swap(VulkanPhysicalDevice &Pd1,
                                                   VulkanPhysicalDevice &Pd2) {
  std::swap(Pd1.Instance, Pd2.Instance);
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    VulkanPhysicalDevice(VulkanPhysicalDevice &&PdToMove) {
  swap(*this, PdToMove);
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT, SceneManagerImplT> &
VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::operator=(VulkanPhysicalDevice &&PdToMove) {
  swap(*this, PdToMove);
  return *this;
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
std::vector<typename VulkanPhysicalDevice<
    WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::PhysicalDeviceLocalProps>
VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT,
                 SceneManagerImplT>::getLocalPhysicalDevices() const {
  if (Instance == nullptr) {
    LOG_F(ERROR, "Instance is not initialized");
    throw std::runtime_error("Instance is not initialized");
  }

  const auto Devices =
      static_cast<vk::Instance>(*Instance).enumeratePhysicalDevices();

  std::vector<PhysicalDeviceLocalProps> LocalDevices;
  std::for_each(
      Devices.begin(), Devices.end(), [&](const vk::PhysicalDevice &Device) {
        const auto Properties = Device.getProperties();
        // Print all properties of device
        LOG_F(INFO, "Physical device properties:");
        LOG_F(INFO, "Device Name: %s",
              std::string(Properties.deviceName).c_str());
        LOG_F(INFO, "Device Type: %d", static_cast<int>(Properties.deviceType));
        LOG_F(INFO, "API Version: %d.%d.%d",
              VK_VERSION_MAJOR(Properties.apiVersion),
              VK_VERSION_MINOR(Properties.apiVersion),
              VK_VERSION_PATCH(Properties.apiVersion));
        LOG_F(INFO, "Driver Version: %d.%d.%d",
              VK_VERSION_MAJOR(Properties.driverVersion),
              VK_VERSION_MINOR(Properties.driverVersion),
              VK_VERSION_PATCH(Properties.driverVersion));
        LOG_F(INFO, "Vendor ID: %d", Properties.vendorID);
        LOG_F(INFO, "Device ID: %d", Properties.deviceID);

        LocalDevices.push_back(PhysicalDeviceLocalProps{
            .Name = std::string(Properties.deviceName),
            .ApiVersion = Properties.apiVersion,
            .DriverVersion = Properties.driverVersion,
            .VendorId = Properties.vendorID,
            .DeviceId = Properties.deviceID,
            .Type = PhysicalDeviceLocalProps::toLocalDeviceType(
                Properties.deviceType)});
      });

  return LocalDevices;
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT,
                      SceneManagerImplT>::findQueueIndexesAndSetup() {
  LOG_F(INFO, "Finding queue indexes");
  if (!PhysicalDevice.has_value()) {
    LOG_F(ERROR, "Physical device is not initialized");
    throw std::runtime_error("Physical device is not initialized");
  }

  std::vector<vk::QueueFamilyProperties> QueueFamilies =
      PhysicalDevice.value().getQueueFamilyProperties();
  for (size_t I = 0; I < QueueFamilies.size(); I++) {
    if (QueueFamilies[I].queueFlags & vk::QueueFlagBits::eGraphics) {
      QueueIndexesInstance.Graphics = I;
    }
    if (QueueFamilies[I].queueFlags & vk::QueueFlagBits::eCompute) {
      QueueIndexesInstance.Present = I;
    }
    if (QueueFamilies[I].queueFlags & vk::QueueFlagBits::eTransfer) {
      QueueIndexesInstance.Transfer = I;
    }

    if (QueueIndexesInstance.isComplete()) {
      LOG_F(INFO, "Queue indexes found");
      break;
    }
  }
  if (!QueueIndexesInstance.isComplete()) {
    LOG_F(ERROR, "Can't find queue indexes");
    throw std::runtime_error("Can't find queue indexes");
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    chooseDeviceAndSetup(
        std::function<bool(const vk::PhysicalDevice &)> Predicate) {
  std::vector<vk::PhysicalDevice> AvailableDevices =
      static_cast<vk::Instance &>(*Instance.get()).enumeratePhysicalDevices();

  std::for_each(
      AvailableDevices.begin(), AvailableDevices.end(), [&](auto Device) {
        if (Predicate(Device)) {
          this->PhysicalDevice = Device;
          LOG_F(INFO, "Physical device chosen %s",
                std::string(Device.getProperties().deviceName).c_str());
        }
      });

  if (!PhysicalDevice.has_value()) {
    if (AvailableDevices.size() > 0) {
      PhysicalDevice = AvailableDevices[0];
      LOG_F(INFO, "Physical device chosen %s",
            std::string(PhysicalDevice->getProperties().deviceName).c_str());
      return;
    }
    LOG_F(ERROR, "Can't setup device with required predicate");
    throw std::runtime_error("Can't setup device with required predicate");
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT,
                      SceneManagerImplT>::setupLogicalDevice() {
  LOG_F(INFO, "Setting up logical device");
  if (!PhysicalDevice.has_value()) {
    LOG_F(ERROR, "Physical device is not initialized");
    throw std::runtime_error("Physical device is not initialized");
  }

  std::vector<vk::DeviceQueueCreateInfo> QueueCreateInfos;
  std::set<uint32_t> UniqueQueueFamilies = {
      QueueIndexesInstance.Graphics.value(),
      QueueIndexesInstance.Present.value(),
      QueueIndexesInstance.Transfer.value()};

  float QueuePriority = 1.0f;
  for (uint32_t QueueFamily : UniqueQueueFamilies) {
    vk::DeviceQueueCreateInfo QueueCreateInfo = {
        .queueFamilyIndex = QueueFamily,
        .queueCount = 1,
        .pQueuePriorities = &QueuePriority,
    };
    QueueCreateInfos.push_back(QueueCreateInfo);
  }

  std::vector<const char *> EnabledExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  if (this->Instance->isPortabilityRequired()) {
    std::vector<vk::ExtensionProperties> Props =
        PhysicalDevice->enumerateDeviceExtensionProperties();
    const auto FoundIt = std::find_if(
        Props.begin(), Props.end(), [](const vk::ExtensionProperties &Prop) {
          return std::string(Prop.extensionName) == "VK_KHR_portability_subset";
        });
    if (FoundIt == Props.end()) {
      LOG_F(WARNING, "Portability extension is not supported");
    } else {
      EnabledExtensions.push_back("VK_KHR_portability_subset");
    }
  }

  std::vector<const char *> EnabledLayers;
  if (VulkanPhysicalDevice::Debug) {
    EnabledLayers.push_back("VK_LAYER_KHRONOS_validation");
  }

  vk::PhysicalDeviceFeatures DeviceFeatures;
  vk::DeviceCreateInfo DeviceCreateInfo = {
      .queueCreateInfoCount = static_cast<uint32_t>(QueueCreateInfos.size()),
      .pQueueCreateInfos = QueueCreateInfos.data(),
      .enabledLayerCount = static_cast<uint32_t>(EnabledLayers.size()),
      .ppEnabledLayerNames =
          EnabledLayers.empty() ? nullptr : EnabledLayers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(EnabledExtensions.size()),
      .ppEnabledExtensionNames =
          EnabledExtensions.empty() ? nullptr : EnabledExtensions.data(),
      .pEnabledFeatures = &DeviceFeatures};

  try {
    LogicalDevice = PhysicalDevice.value().createDevice(DeviceCreateInfo);
    LOG_F(INFO, "Logical device set up");
  } catch (const std::exception &E) {
    LOG_F(ERROR, "Can't set up logical device: %s", E.what());
    throw;
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT,
                      SceneManagerImplT>::setupQueues() {
  if (!LogicalDevice.has_value()) {
    LOG_F(ERROR, "Logical device is not initialized");
    throw std::runtime_error("Logical device is not initialized");
  }

  if (QueueIndexesInstance.Graphics.has_value()) {
    LOG_F(INFO, "Setting up graphics queue");
    GraphicsQueue = std::make_shared<vk::Queue>(
        LogicalDevice->getQueue(QueueIndexesInstance.Graphics.value(), 0));
  } else {
    LOG_F(WARNING, "Graphics queue is not initialized");
  }

  if (QueueIndexesInstance.Present.has_value()) {
    LOG_F(INFO, "Setting up present queue");
    PresentQueue = std::make_shared<vk::Queue>(
        LogicalDevice->getQueue(QueueIndexesInstance.Present.value(), 0));
  } else {
    LOG_F(WARNING, "Present queue is not initialized");
  }
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    setupPipeline(
        const VulkanPhysicalDevice::PipelineInitDataStruct &&PipelineInitData) {
  std::map<vk::QueueFlagBits, uint32_t> FamilyIndexes;
  if (QueueIndexesInstance.Graphics.has_value()) {
    LOG_F(INFO, "Graphics family index: %d",
          QueueIndexesInstance.Graphics.value());
    FamilyIndexes[vk::QueueFlagBits::eGraphics] =
        QueueIndexesInstance.Graphics.value();
  }
  if (QueueIndexesInstance.Present.has_value()) {
    LOG_F(INFO, "Present family index: %d",
          QueueIndexesInstance.Present.value());
    FamilyIndexes[vk::QueueFlagBits::eCompute] =
        QueueIndexesInstance.Present.value();
  }
  if (QueueIndexesInstance.Transfer.has_value()) {
    LOG_F(INFO, "Transfer family index: %d",
          QueueIndexesInstance.Transfer.value());
    FamilyIndexes[vk::QueueFlagBits::eTransfer] =
        QueueIndexesInstance.Transfer.value();
  }

  typename vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT,
                               SceneManagerImplT>::VkPipelineProps Props{
      .PhysicalDevice = PipelineInitData.ThisPhysicalDevice,
      .Instance = this->Instance,
      .Facades = {.Window = PipelineInitData.Window,
                  .ShaderLoader = PipelineInitData.ShaderLoader,
                  .SceneManager = PipelineInitData.SceneManager},
      .FamilyIndexes = std::move(FamilyIndexes),
      .GraphicsQueue = PipelineInitData.ThisPhysicalDevice->GraphicsQueue,
      .PresentQueue = PipelineInitData.ThisPhysicalDevice->PresentQueue,
  };
  this->Pipeline = std::make_unique<
      vk_core::VulkanPipeline<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>>(
      std::move(Props));
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT,
                      SceneManagerImplT>::render() {
  if (this->Pipeline == nullptr) {
    LOG_F(ERROR, "Pipeline is not initialized");
    throw std::runtime_error("Pipeline is not initialized");
  }
  this->Pipeline->render();
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    addObjectData(
        const std::map<std::string, typename vk_core::VulkanPipeline<
                                        WindowImpl, ShaderLoaderImplT,
                                        SceneManagerImplT>::PublicObjectData>
            &Dump,
        const std::string &DumpName) {
  if (this->Pipeline == nullptr) {
    LOG_F(ERROR, "Pipeline is not initialized");
    throw std::runtime_error("Pipeline is not initialized");
  }
  this->Pipeline->addObjectData(Dump, DumpName);
}

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderLoaderImplT,
          SceneManagerImpl<scene_manager_facades::SceneManagerDependencies>
              SceneManagerImplT>
void VulkanPhysicalDevice<WindowImpl, ShaderLoaderImplT, SceneManagerImplT>::
    addTexture(const std::string &TexturePath, const std::string &TextureName) {
  if (this->Pipeline == nullptr) {
    LOG_F(ERROR, "Pipeline is not initialized");
    throw std::runtime_error("Pipeline is not initialized");
  }
  this->Pipeline->addTexture(TexturePath, TextureName);
}

template class vk_core::VulkanPhysicalDevice<
    window_api_impls::WindowApiFacadeGlfwImpl,
    shader_loader_impls::ShaderLoaderSimpleImpl,
    scene_manager_facades::SceneManagerBaseImpl<
        scene_manager_facades::SceneManagerDependencies>>;

} // namespace vk_core