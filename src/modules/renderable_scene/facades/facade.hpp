//
// Created by FullHat on 13/05/2024.
//

#ifndef ENGINE_SRC_MODULES_RENDERABLE_SCENE_FACADES_FACADE_HPP
#define ENGINE_SRC_MODULES_RENDERABLE_SCENE_FACADES_FACADE_HPP

#include "../../graphics/facades/facade.hpp"
#include "../../scene_manager/facades/facade.hpp"
#include <concepts>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Define all structures to be used in the facades
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct RenderableSceneFacadeStructs {
  template <typename DependencyStructT> struct RenderableSceneProps {
    // Add your properties here
    DependencyStructT Dependencies;
  };
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Concept to get errors earlier if the Impl is not valid
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename T, typename DependencyStructT>
concept RenderableSceneImpl = requires(T Obj) {
  std::is_constructible_v<
      T, RenderableSceneFacadeStructs::RenderableSceneProps<DependencyStructT>>;
  // Add your requirements here
  { Obj.runMainCycle() };
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blueprint for the RenderableScene Implementation, add functions here
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename DependencyStructT>
concept RenderableSceneDependenciesConcept = requires(DependencyStructT Dep) {
  { Dep.Graphics };
  { Dep.SceneManager };
};

#define RENDERABLE_SCENE_IMPL(name, DependencyStructConcept)                   \
  template <DependencyStructConcept Dep> class RenderableScene##name##Impl {   \
  public:                                                                      \
    RenderableScene##name##Impl(                                               \
        RenderableSceneFacadeStructs::RenderableSceneProps<Dep> &&);           \
    ~RenderableScene##name##Impl();                                            \
                                                                               \
    void runMainCycle();                                                       \
                                                                               \
  protected:                                                                   \
    void *Data;                                                                \
  };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Ready for use implementations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace renderable_scene_facades {
template <typename SceneManagerDepT,
          SceneManagerImpl<SceneManagerDepT> SceneManagerT,
          typename GraphicsDepT, GraphicApiImpl<GraphicsDepT> GraphicsT, WindowApiImpl WindowT>
struct RenderableSceneDependencies {
  std::shared_ptr<SceneManagerFacade<SceneManagerDepT, SceneManagerT>>
      SceneManager;
  std::shared_ptr<GraphicApiFacade<GraphicsDepT, GraphicsT>> Graphics;
  std::shared_ptr<WindowApiFacade<WindowT>> Window;

  using SceneManagerType = SceneManagerT;
  using GraphicsType = GraphicsT;
  using WindowType = WindowT;
};
RENDERABLE_SCENE_IMPL(Base, RenderableSceneDependenciesConcept);
} // namespace renderable_scene_facades

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Facade for the RenderableScene
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename DependencyStructT,
          RenderableSceneImpl<DependencyStructT> Impl>
class RenderableSceneFacade {
public:
  RenderableSceneFacade(
      RenderableSceneFacadeStructs::RenderableSceneProps<DependencyStructT>
          &&Props)
      : ImplInstance(std::move(Props)) {}

  Impl ImplInstance;
};

#endif // ENGINE_SRC_MODULES_RENDERABLE_SCENE_FACADES_FACADE_HPP
