//
// Created by FullHat on 02/04/2024.
//

#ifndef ENGINE_SRC_MODULES_VK_CORE_FACADES_FACADE_HPP
#define ENGINE_SRC_MODULES_VK_CORE_FACADES_FACADE_HPP

#include "../../../adapters_interfaces/fabric_adapter.hpp"
#include "../src/GraphicEngine.hpp"
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

  template <WindowApiImpl WindowImpl> struct GraphicEngineProps {
    WindowApiFacade<WindowImpl> Window;

    InstanceProps VkInstanceProps;
    PhysicalDeviceProps VkPhysicalDeviceProps;
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

template <typename T, typename WindowImpl>
concept GraphicApiImpl = requires(T Obj) {
  std::is_constructible_v<
      T, GraphicFacadeStructs::GraphicEngineProps<WindowImpl> &&>;
  {
    Obj.getLocalPhysicalDevices()
  } -> std::same_as<std::vector<GraphicFacadeStructs::PhysicalDeviceData>>;
  {
    Obj.chooseGpu(std::declval<GraphicFacadeStructs::PhysicalDeviceData>())
  } -> std::same_as<void>;
  {
    Obj.chooseGpu(std::declval<GraphicFacadeStructs::DeviceChoosePolicy>(),
                  std::declval<bool>())
  } -> std::same_as<void>;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blueprint for the GraphicApi Implementation, add functions here
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define GRAPHIC_API_IMPL(name)                                                 \
  template <WindowApiImpl WindowImpl> class GraphicApiFacade##name##Impl {     \
  public:                                                                      \
    GraphicApiFacade##name##Impl(const GraphicApiFacade##name##Impl &&) =      \
        delete;                                                                \
    GraphicApiFacade##name##Impl(                                              \
        GraphicFacadeStructs::GraphicEngineProps<WindowImpl> &&);              \
    ~GraphicApiFacade##name##Impl();                                           \
                                                                               \
    std::vector<GraphicFacadeStructs::PhysicalDeviceData>                      \
    getLocalPhysicalDevices() const;                                           \
                                                                               \
    void                                                                       \
    chooseGpu(const GraphicFacadeStructs::PhysicalDeviceData &DeviceData);     \
                                                                               \
    void                                                                       \
    chooseGpu(const GraphicFacadeStructs::DeviceChoosePolicy ChoosePolicy,     \
              const bool TryChooseAnyIfFailed);                                \
                                                                               \
  protected:                                                                   \
    void *Data;                                                                \
  };
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Ready to use implementations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace graphic_api_impls {
GRAPHIC_API_IMPL(Vulkan)
} // namespace graphic_api_impls

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Facade for the GraphicApi
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <WindowApiImpl WindowImpl>
class GraphicApiFacade {
public:
  using Impl = graphic_api_impls::GraphicApiFacadeVulkanImpl<WindowImpl>;

  GraphicApiFacade(GraphicFacadeStructs::GraphicEngineProps<WindowImpl> &&Props)
      : ImplInstance(std::move(Props)) {}

public:
  [[maybe_unused]] Impl ImplInstance;
};

[[maybe_unused]]
typedef graphic_api_impls::GraphicApiFacadeVulkanImpl<
    window_api_impls::WindowApiFacadeGlfwImpl>
    VkTemplateInstantiation; // нигде не используется

#endif // ENGINE_SRC_MODULES_VK_CORE_FACADES_FACADE_HPP
