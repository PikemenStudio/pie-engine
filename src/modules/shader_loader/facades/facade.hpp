//
// Created by FullHat on 09/05/2024.
//

#ifndef ENGINE_SRC_MODULES_SHADER_LOADER_FACADES_FACADE_HPP
#define ENGINE_SRC_MODULES_SHADER_LOADER_FACADES_FACADE_HPP

#include <optional>
#include <sstream>
#include <string>
#include <vector>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Define all structures to be used in the facades
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct ShaderLoaderFacadeStructs {
  struct ShaderProps {
    std::optional<std::string> CompilerPath; // Required to compile shaders
    std::optional<std::string> CacheFolder;  // Required to compile shaders
  };

  struct ShadersLocations {
    struct ShaderInitData {
      std::string ShaderName;
      std::string ShaderPath;
    };
    std::vector<ShaderInitData> Shaders;
  };

  struct ShaderData {
    std::string Name;
    std::stringstream Data;
  };
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Concept to get errors earlier if the Impl is not valid
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename T>
concept ShaderLoaderImpl = requires(T ImplInstance) {
  {
    ImplInstance.loadAndCompileShaders(
        ShaderLoaderFacadeStructs::ShadersLocations{})
  } -> std::same_as<std::vector<ShaderLoaderFacadeStructs::ShaderData>>;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blueprint for the ShaderLoader Implementation, add functions here
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define SHADER_LOADER_IMPL(Name)                                               \
  class ShaderLoader##Name##Impl {                                             \
  public:                                                                      \
    ShaderLoader##Name##Impl(const ShaderLoader##Name##Impl &) = delete;       \
    ShaderLoader##Name##Impl(ShaderLoader##Name##Impl &&) = default;           \
    ShaderLoader##Name##Impl &                                                 \
    operator=(const ShaderLoader##Name##Impl &) = delete;                      \
    ShaderLoader##Name##Impl &                                                 \
    operator=(ShaderLoader##Name##Impl &&) = default;                          \
    ShaderLoader##Name##Impl(ShaderLoaderFacadeStructs::ShaderProps &&);       \
    ~ShaderLoader##Name##Impl();                                               \
                                                                               \
    std::vector<ShaderLoaderFacadeStructs::ShaderData> loadAndCompileShaders(  \
        const ShaderLoaderFacadeStructs::ShadersLocations &Locs);              \
                                                                               \
  protected:                                                                   \
    void *Data;                                                                \
  };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Ready to use implementations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace shader_loader_impls {
SHADER_LOADER_IMPL(Simple)
} // namespace shader_loader_impls

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Facade for the ShaderLoader
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <ShaderLoaderImpl Impl> class ShaderLoaderFacade {
public:
  ShaderLoaderFacade(ShaderLoaderFacadeStructs::ShaderProps &&Props)
      : ImplInstance(std::move(Props)) {}

public:
  [[maybe_unused]] Impl ImplInstance;
};

#endif // ENGINE_SRC_MODULES_SHADER_LOADER_FACADES_FACADE_HPP
