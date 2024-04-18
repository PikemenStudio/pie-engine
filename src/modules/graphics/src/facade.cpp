#include "../facades/facade.hpp"
#include "GraphicEngine.hpp"

using namespace graphic_api_impls;

template <WindowApiImpl WindowImpl>
using DataType = vk_core::GraphicEngine<WindowImpl>;

template <WindowApiImpl WindowImpl> using DataTypePtr = DataType<WindowImpl> *;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convertors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <WindowApiImpl WindowImpl>
vk_core::GraphicEngine<WindowImpl>::GraphicEngineProps
toModuleType(GraphicFacadeStructs::GraphicEngineProps<WindowImpl> &&Props) {
  typename vk_core::GraphicEngine<WindowImpl>::GraphicEngineProps ModuleProps{
      .Window = std::move(Props.Window),
      .VkInstanceProps =
          {
              .AppName = std::move(Props.InstancePropsInstance.AppName),
              .EngineName = std::move(Props.InstancePropsInstance.EngineName),
              .AppVersion =
                  {
                      .Major = Props.InstancePropsInstance.AppVersion.Major,
                      .Minor = Props.InstancePropsInstance.AppVersion.Minor,
                      .Patch = Props.InstancePropsInstance.AppVersion.Patch,
                  },
              .EngineVersion =
                  {
                      .Major = Props.InstancePropsInstance.EngineVersion.Major,
                      .Minor = Props.InstancePropsInstance.EngineVersion.Minor,
                      .Patch = Props.InstancePropsInstance.EngineVersion.Patch,
                  },
              .RequestedWindowExtensions =
                  std::move(Props.InstancePropsInstance.RequestedWindowExtensions),
          },
      .VkPhysicalDeviceProps = {
          // Just empty :)
      }};
  return ModuleProps;
}

GraphicFacadeStructs::PhysicalDeviceData::TypeEnum toExternalType(
    vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps::TypeEnum Type) {
  switch (Type) {
  case vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps::TypeEnum::OTHER:
    return GraphicFacadeStructs::PhysicalDeviceData::TypeEnum::OTHER;
  case vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps::TypeEnum::
      INTEGRATED_GPU:
    return GraphicFacadeStructs::PhysicalDeviceData::TypeEnum::INTEGRATED_GPU;
  case vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps::TypeEnum::
      DISCRETE_GPU:
    return GraphicFacadeStructs::PhysicalDeviceData::TypeEnum::DISCRETE_GPU;
  case vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps::TypeEnum::
      VIRTUAL_GPU:
    return GraphicFacadeStructs::PhysicalDeviceData::TypeEnum::VIRTUAL_GPU;
  case vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps::TypeEnum::CPU:
    return GraphicFacadeStructs::PhysicalDeviceData::TypeEnum::CPU;
  default:
    return GraphicFacadeStructs::PhysicalDeviceData::TypeEnum::OTHER;
  }
}

vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps::TypeEnum
toModuleType(GraphicFacadeStructs::PhysicalDeviceData::TypeEnum Type) {
  switch (Type) {
  case GraphicFacadeStructs::PhysicalDeviceData::TypeEnum::OTHER:
    return vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps::TypeEnum::OTHER;
  case GraphicFacadeStructs::PhysicalDeviceData::TypeEnum::INTEGRATED_GPU:
    return vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps::TypeEnum::
        INTEGRATED_GPU;
  case GraphicFacadeStructs::PhysicalDeviceData::TypeEnum::DISCRETE_GPU:
    return vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps::TypeEnum::
        DISCRETE_GPU;
  case GraphicFacadeStructs::PhysicalDeviceData::TypeEnum::VIRTUAL_GPU:
    return vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps::TypeEnum::
        VIRTUAL_GPU;
  case GraphicFacadeStructs::PhysicalDeviceData::TypeEnum::CPU:
    return vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps::TypeEnum::CPU;
  default:
    return vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps::TypeEnum::OTHER;
  }
}

template <WindowApiImpl WindowImpl>
vk_core::GraphicEngine<WindowImpl>::DeviceChoosePolicy
toModuleType(GraphicFacadeStructs::DeviceChoosePolicy Policy) {
  switch (Policy) {
  case GraphicFacadeStructs::DeviceChoosePolicy::FIRST:
    return vk_core::GraphicEngine<WindowImpl>::DeviceChoosePolicy::FIRST;
  case GraphicFacadeStructs::DeviceChoosePolicy::BEST:
    return vk_core::GraphicEngine<WindowImpl>::DeviceChoosePolicy::BEST;
  default:
    return vk_core::GraphicEngine<WindowImpl>::DeviceChoosePolicy::FIRST;
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Implementations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <WindowApiImpl WindowImpl>
GraphicApiFacadeVulkanImpl<WindowImpl>::GraphicApiFacadeVulkanImpl(
    GraphicFacadeStructs::GraphicEngineProps<WindowImpl> &&Props) {
  Data = new DataType<WindowImpl>(toModuleType(std::move(Props)));
}

template <WindowApiImpl WindowImpl>
GraphicApiFacadeVulkanImpl<WindowImpl>::~GraphicApiFacadeVulkanImpl() {
  delete static_cast<DataTypePtr<WindowImpl>>(Data);
}

template <WindowApiImpl WindowImpl>
std::vector<GraphicFacadeStructs::PhysicalDeviceData>
GraphicApiFacadeVulkanImpl<WindowImpl>::getLocalPhysicalDevices() const {
  std::vector<GraphicFacadeStructs::PhysicalDeviceData> Result;
  for (const auto &Device :
       static_cast<DataTypePtr<WindowImpl>>(Data)->getLocalPhysicalDevices()) {
    Result.push_back(GraphicFacadeStructs::PhysicalDeviceData{
        .Name = Device.Name,
        .ApiVersion = Device.ApiVersion,
        .DriverVersion = Device.DriverVersion,
        .VendorId = Device.VendorId,
        .DeviceId = Device.DeviceId,
        .Type = toExternalType(Device.Type),
    });
  }
  return Result;
}

template <WindowApiImpl WindowImpl>
void GraphicApiFacadeVulkanImpl<WindowImpl>::chooseGpu(
    const GraphicFacadeStructs::PhysicalDeviceData &DeviceData) {
  vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps Device{
      .Name = DeviceData.Name,
      .ApiVersion = DeviceData.ApiVersion,
      .DriverVersion = DeviceData.DriverVersion,
      .VendorId = DeviceData.VendorId,
      .DeviceId = DeviceData.DeviceId,
      .Type = toModuleType(DeviceData.Type),
  };
  static_cast<DataTypePtr<WindowImpl>>(Data)->chooseLocalPhysicalDevice(Device);
}

template <WindowApiImpl WindowImpl>
void GraphicApiFacadeVulkanImpl<WindowImpl>::chooseGpu(
    const GraphicFacadeStructs::DeviceChoosePolicy Policy) {
  static_cast<DataTypePtr<WindowImpl>>(Data)->chooseLocalPhysicalDevice(
      toModuleType<WindowImpl>(Policy));
}

// Explicitly instantiate class
template class graphic_api_impls::GraphicApiFacadeVulkanImpl<
    window_api_impls::WindowApiFacadeGlfwImpl>;
