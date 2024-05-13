//
// Created by FullHat on 13/05/2024.
//

#ifndef ENGINE_SRC_MODULES_SCENE_MANAGER_FACADES_FACADE_HPP
#define ENGINE_SRC_MODULES_SCENE_MANAGER_FACADES_FACADE_HPP

#include <concepts>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Define all structures to be used in the facades
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct SceneManagerFacadeStructs {
  template <typename DependencyStructT> struct SceneManagerProps {
    // Add your properties here
    DependencyStructT Dependencies;
  };
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Concept to get errors earlier if the Impl is not valid
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename T, typename DependencyStructT>
concept SceneManagerImpl = requires(T Obj) {
  std::is_constructible_v<
      T, SceneManagerFacadeStructs::SceneManagerProps<DependencyStructT>>;
  // Add your requirements here
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blueprint for the SceneManager Implementation, add functions here
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename DependencyStructT>
concept SceneManagerDependenciesConcept = requires(DependencyStructT Dep) {
  { true };
};

#define SCENE_MANAGER_IMPL(name, DependencyStructConcept)                      \
  template <DependencyStructConcept Dep> class SceneManager##name##Impl {      \
  public:                                                                      \
    using DepType = Dep;                                                       \
                                                                               \
    SceneManager##name##Impl(                                                  \
        SceneManagerFacadeStructs::SceneManagerProps<Dep> &&);                 \
    ~SceneManager##name##Impl();                                               \
                                                                               \
  protected:                                                                   \
    void *Data;                                                                \
  };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Ready for use implementations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace scene_manager_facades {
struct SceneManagerDependencies {};
SCENE_MANAGER_IMPL(Base, SceneManagerDependenciesConcept)
} // namespace scene_manager_facades

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Facade to be used by the user
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename DependencyStructT,
          SceneManagerImpl<DependencyStructT> Impl =
              scene_manager_facades::SceneManagerBaseImpl<DependencyStructT>>
class SceneManagerFacade {
public:
  using DepType = DependencyStructT;

  SceneManagerFacade(
      SceneManagerFacadeStructs::SceneManagerProps<DependencyStructT> &&Props)
      : ImplInstance(std::move(Props)) {}

protected:
  Impl ImplInstance;
};

#endif // ENGINE_SRC_MODULES_SCENE_MANAGER_FACADES_FACADE_HPP
