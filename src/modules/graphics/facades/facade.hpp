//
// Created by FullHat on 02/04/2024.
//

#ifndef ENGINE_SRC_MODULES_VK_CORE_FACADES_FACADE_HPP
#define ENGINE_SRC_MODULES_VK_CORE_FACADES_FACADE_HPP

#include "../../shader_loader/facades/facade.hpp"
#include "../../windows/facades/facade.hpp"
#include <any>
#include <concepts>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Define all structures to be used in the facades
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct GraphicFacadeStructs {
  struct InstanceProps {
    struct Version {
      uint32_t Major;
      uint32_t Minor;
      uint32_t Patch;
    };

    std::string AppName;
    std::string EngineName;
    Version AppVersion;
    Version EngineVersion;

    // Window dependency
    std::vector<const char *> RequestedWindowExtensions;
  };

  struct PhysicalDeviceProps {};

  template <typename DependencyStructT> struct GraphicEngineProps {
    DependencyStructT Dependencies;

    InstanceProps InstancePropsInstance;
    PhysicalDeviceProps PhysicalDevicePropsInstance;
  };

  struct PhysicalDeviceData {
    std::string Name;
    uint32_t ApiVersion;
    uint32_t DriverVersion;
    uint32_t VendorId;
    uint32_t DeviceId;

    enum class TypeEnum {
      OTHER = 0,
      INTEGRATED_GPU = 1,
      DISCRETE_GPU = 2,
      VIRTUAL_GPU = 3,
      CPU = 4,
    } Type;
  };

  enum class DeviceChoosePolicy : uint_fast8_t {
    FIRST,
    BEST,
  };
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Concept to get errors earlier if the Impl is not valid
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename T, typename DependencyStructT>
concept GraphicApiImpl = requires(T Obj) {
  std::is_constructible_v<
      T, GraphicFacadeStructs::GraphicEngineProps<DependencyStructT> &&>;
  {
    Obj.getLocalPhysicalDevices()
  } -> std::same_as<std::vector<GraphicFacadeStructs::PhysicalDeviceData>>;
  {
    Obj.chooseGpu(std::declval<GraphicFacadeStructs::PhysicalDeviceData>())
  } -> std::same_as<void>;
  {
    Obj.chooseGpu(std::declval<GraphicFacadeStructs::DeviceChoosePolicy>())
  } -> std::same_as<void>;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blueprint for the GraphicApi Implementation, add functions here
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Define all concepts with implementation dependencies
template <typename DependencyStructT>
concept VulkanDependenciesConcept = requires(DependencyStructT Dep) {
  { Dep.Window };
};

#define GRAPHIC_API_IMPL(name, DependencyStructConcept)                        \
  template <DependencyStructConcept Dep> class GraphicApiFacade##name##Impl {  \
  public:                                                                      \
    GraphicApiFacade##name##Impl(const GraphicApiFacade##name##Impl &&) =      \
        delete;                                                                \
    GraphicApiFacade##name##Impl(                                              \
        GraphicFacadeStructs::GraphicEngineProps<Dep> &&);                     \
    ~GraphicApiFacade##name##Impl();                                           \
                                                                               \
    std::vector<GraphicFacadeStructs::PhysicalDeviceData>                      \
    getLocalPhysicalDevices() const;                                           \
                                                                               \
    void                                                                       \
    chooseGpu(const GraphicFacadeStructs::PhysicalDeviceData &DeviceData);     \
                                                                               \
    void                                                                       \
    chooseGpu(const GraphicFacadeStructs::DeviceChoosePolicy ChoosePolicy);    \
                                                                               \
  protected:                                                                   \
    void *Data;                                                                \
  };
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Ready to use implementations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace graphic_api_impls {
template <WindowApiImpl WindowT, ShaderLoaderImpl ShaderLoaderT>
struct VulkanDependencies {
  using WindowType = WindowT;
  WindowApiFacade<WindowT> Window;

  using ShaderLoaderType = ShaderLoaderT;
  ShaderLoaderFacade<ShaderLoaderT> ShaderLoader;
};
GRAPHIC_API_IMPL(Vulkan, VulkanDependenciesConcept)
} // namespace graphic_api_impls

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Facade for the GraphicApi
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename DependencyStructT, GraphicApiImpl<DependencyStructT> Impl>
class GraphicApiFacade {
public:
  GraphicApiFacade(
      GraphicFacadeStructs::GraphicEngineProps<DependencyStructT> &&Props)
      : ImplInstance(std::move(Props)) {}

public:
  [[maybe_unused]] Impl ImplInstance;
};

#endif // ENGINE_SRC_MODULES_VK_CORE_FACADES_FACADE_HPP
