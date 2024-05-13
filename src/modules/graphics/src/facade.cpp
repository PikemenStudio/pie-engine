#include "../facades/facade.hpp"
#include "GraphicEngine.hpp"

using namespace graphic_api_impls;

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderImpl>
using DataType = vk_core::GraphicEngine<WindowImpl, ShaderImpl>;

template <WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderImpl>
using DataTypePtr = DataType<WindowImpl, ShaderImpl> *;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convertors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <VulkanDependenciesConcept Dep>
vk_core::GraphicEngine<typename Dep::WindowType,
                       typename Dep::ShaderLoaderType>::DeviceChoosePolicy
toModuleType(GraphicFacadeStructs::DeviceChoosePolicy Policy) {
  switch (Policy) {
  case GraphicFacadeStructs::DeviceChoosePolicy::FIRST:
    return vk_core::GraphicEngine<
        typename Dep::WindowType,
        typename Dep::ShaderLoaderType>::DeviceChoosePolicy::FIRST;
  case GraphicFacadeStructs::DeviceChoosePolicy::BEST:
    return vk_core::GraphicEngine<
        typename Dep::WindowType,
        typename Dep::ShaderLoaderType>::DeviceChoosePolicy::BEST;
  default:
    return vk_core::GraphicEngine<
        typename Dep::WindowType,
        typename Dep::ShaderLoaderType>::DeviceChoosePolicy::FIRST;
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
      typename Dep::WindowType,
      typename Dep::ShaderLoaderType>::GraphicEngineProps ModuleProps{
      .Window = std::move(Props.Dependencies.Window),
      .ShaderLoader = std::move(Props.Dependencies.ShaderLoader),
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
  Data = new vk_core::GraphicEngine<typename Dep::WindowType,
                                    typename Dep::ShaderLoaderType>(
      std::move(ModuleProps));
}

template <VulkanDependenciesConcept Dep>
GraphicApiFacadeVulkanImpl<Dep>::~GraphicApiFacadeVulkanImpl() {
  delete static_cast<
      DataTypePtr<typename Dep::WindowType, typename Dep::ShaderLoaderType>>(
      Data);
}

template <VulkanDependenciesConcept Dep>
std::vector<GraphicFacadeStructs::PhysicalDeviceData>
GraphicApiFacadeVulkanImpl<Dep>::getLocalPhysicalDevices() const {
  std::vector<GraphicFacadeStructs::PhysicalDeviceData> Result;
  for (const auto &Device :
       static_cast<DataTypePtr<typename Dep::WindowType,
                               typename Dep::ShaderLoaderType>>(Data)
           ->getLocalPhysicalDevices()) {
    Result.push_back(GraphicFacadeStructs::PhysicalDeviceData{
        .Name = Device.Name,
        .ApiVersion = Device.ApiVersion,
        .DriverVersion = Device.DriverVersion,
        .VendorId = Device.VendorId,
        .DeviceId = Device.DeviceId,
        .Type = static_cast<GraphicFacadeStructs::PhysicalDeviceData::TypeEnum>(
            Device.Type),
    });
  }
  return Result;
}

template <VulkanDependenciesConcept Dep>
void GraphicApiFacadeVulkanImpl<Dep>::chooseGpu(
    const GraphicFacadeStructs::PhysicalDeviceData &DeviceData) {
  typename vk_core::VkPhysicalDevice<
      typename Dep::WindowType,
      typename Dep::ShaderLoaderType>::PhysicalDeviceLocalProps Device{
      .Name = DeviceData.Name,
      .ApiVersion = DeviceData.ApiVersion,
      .DriverVersion = DeviceData.DriverVersion,
      .VendorId = DeviceData.VendorId,
      .DeviceId = DeviceData.DeviceId,
      .Type = static_cast<vk_core::VkPhysicalDevice<
          typename Dep::WindowType,
          typename Dep::ShaderLoaderType>::PhysicalDeviceLocalProps::TypeEnum>(
          DeviceData.Type),
  };
  static_cast<
      DataTypePtr<typename Dep::WindowType, typename Dep::ShaderLoaderType>>(
      Data)
      ->chooseLocalPhysicalDevice(Device);
}

template <VulkanDependenciesConcept Dep>
void GraphicApiFacadeVulkanImpl<Dep>::chooseGpu(
    const GraphicFacadeStructs::DeviceChoosePolicy Policy) {
  static_cast<
      DataTypePtr<typename Dep::WindowType, typename Dep::ShaderLoaderType>>(
      Data)
      ->chooseLocalPhysicalDevice(toModuleType<Dep>(Policy));
}

// Explicitly instantiate class
template class graphic_api_impls::GraphicApiFacadeVulkanImpl<
    graphic_api_impls::VulkanDependencies<
        window_api_impls::WindowApiFacadeGlfwImpl,
        shader_loader_impls::ShaderLoaderSimpleImpl>>;
