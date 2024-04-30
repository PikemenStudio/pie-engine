#include "../facades/facade.hpp"
#include "GraphicEngine.hpp"

using namespace graphic_api_impls;

template <WindowApiImpl WindowImpl>
using DataType = vk_core::GraphicEngine<WindowImpl>;

template <WindowApiImpl WindowImpl> using DataTypePtr = DataType<WindowImpl> *;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convertors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

template <VulkanDependenciesConcept Dep>
vk_core::GraphicEngine<typename Dep::WindowType>::DeviceChoosePolicy
toModuleType(GraphicFacadeStructs::DeviceChoosePolicy Policy) {
  switch (Policy) {
  case GraphicFacadeStructs::DeviceChoosePolicy::FIRST:
    return vk_core::GraphicEngine<
        typename Dep::WindowType>::DeviceChoosePolicy::FIRST;
  case GraphicFacadeStructs::DeviceChoosePolicy::BEST:
    return vk_core::GraphicEngine<
        typename Dep::WindowType>::DeviceChoosePolicy::BEST;
  default:
    return vk_core::GraphicEngine<
        typename Dep::WindowType>::DeviceChoosePolicy::FIRST;
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Implementations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <VulkanDependenciesConcept Dep>
GraphicApiFacadeVulkanImpl<Dep>::GraphicApiFacadeVulkanImpl(
    GraphicFacadeStructs::GraphicEngineProps<Dep> &&Props) {

  // Convert
  // From GraphicFacadeStructs::GraphicEngineProps<Dep> &&Props
  // To   vk_core::GraphicEngine<Dep::WindowType>::GraphicEngineProps
  typename vk_core::GraphicEngine<
      typename Dep::WindowType>::GraphicEngineProps ModuleProps{
      .Window = std::move(Props.Dependencies.Window),
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
              .RequestedWindowExtensions = std::move(
                  Props.InstancePropsInstance.RequestedWindowExtensions),
          },
      .VkPhysicalDeviceProps = {
          // Just empty :)
      }};

  // And pass to the module
  Data = new vk_core::GraphicEngine<typename Dep::WindowType>(
      std::move(ModuleProps));
}

template <VulkanDependenciesConcept Dep>
GraphicApiFacadeVulkanImpl<Dep>::~GraphicApiFacadeVulkanImpl() {
  delete static_cast<DataTypePtr<typename Dep::WindowType> *>(Data);
}

template <VulkanDependenciesConcept Dep>
std::vector<GraphicFacadeStructs::PhysicalDeviceData>
GraphicApiFacadeVulkanImpl<Dep>::getLocalPhysicalDevices() const {
  std::vector<GraphicFacadeStructs::PhysicalDeviceData> Result;
  for (const auto &Device :
       static_cast<DataTypePtr<typename Dep::WindowType>>(Data)
           ->getLocalPhysicalDevices()) {
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

template <VulkanDependenciesConcept Dep>
void GraphicApiFacadeVulkanImpl<Dep>::chooseGpu(
    const GraphicFacadeStructs::PhysicalDeviceData &DeviceData) {
  vk_core::VkPhysicalDevice::PhysicalDeviceLocalProps Device{
      .Name = DeviceData.Name,
      .ApiVersion = DeviceData.ApiVersion,
      .DriverVersion = DeviceData.DriverVersion,
      .VendorId = DeviceData.VendorId,
      .DeviceId = DeviceData.DeviceId,
      .Type = toModuleType(DeviceData.Type),
  };
  static_cast<DataTypePtr<typename Dep::WindowType>>(Data)
      ->chooseLocalPhysicalDevice(Device);
}

template <VulkanDependenciesConcept Dep>
void GraphicApiFacadeVulkanImpl<Dep>::chooseGpu(
    const GraphicFacadeStructs::DeviceChoosePolicy Policy) {
  static_cast<DataTypePtr<typename Dep::WindowType>>(Data)
      ->chooseLocalPhysicalDevice(toModuleType<Dep>(Policy));
}

// Explicitly instantiate class
template class graphic_api_impls::GraphicApiFacadeVulkanImpl<
    graphic_api_impls::VulkanDependencies<
        window_api_impls::WindowApiFacadeGlfwImpl>>;
