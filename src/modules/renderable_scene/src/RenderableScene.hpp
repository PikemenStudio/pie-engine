//
// Created by FullHat on 13/05/2024.
//

#ifndef ENGINE_SRC_MODULES_RENDERABLE_SCENE_SRC_RENDERABLESCENE_HPP
#define ENGINE_SRC_MODULES_RENDERABLE_SCENE_SRC_RENDERABLESCENE_HPP

#include "../../graphics/facades/facade.hpp"
#include "../../scene_manager/facades/facade.hpp"

namespace rs {

template <typename SceneManagerDepsT,
          SceneManagerImpl<SceneManagerDepsT> SceneManagerT,
          typename GraphicsDepsT, GraphicApiImpl<GraphicsDepsT> GraphicsT,
          WindowApiImpl WindowT>
class RenderableScene {
public:
  struct RenderableSceneDependencies {
    std::shared_ptr<SceneManagerFacade<SceneManagerDepsT, SceneManagerT>>
        SceneManager;
    std::shared_ptr<GraphicApiFacade<GraphicsDepsT, GraphicsT>> Graphics;
    std::shared_ptr<WindowApiFacade<WindowT>> Window;
  };
  struct RenderableSceneProps {
    RenderableSceneDependencies Dependencies;
  };

  RenderableScene(RenderableSceneProps &&Props) : Data(std::move(Props)) {}

  ~RenderableScene() = default;

  void runMainCycle();

protected:
  RenderableSceneProps Data;
};

#include "GLFW/glfw3.h"

template <typename SceneManagerDepsT,
          SceneManagerImpl<SceneManagerDepsT> SceneManagerT,
          typename GraphicsDepsT, GraphicApiImpl<GraphicsDepsT> GraphicsT,
          WindowApiImpl WindowT>
void RenderableScene<SceneManagerDepsT, SceneManagerT, GraphicsDepsT, GraphicsT,
                     WindowT>::runMainCycle() {
  auto &Graphics = Data.Dependencies.Graphics;

  WindowT &Window = Data.Dependencies.Window->ImplInstance;

  Window.prepare();
  while (!Window.shouldClose()) {
    Window.update();
    Window.swapBuffers();
  }
}

} // namespace rs

#endif // ENGINE_SRC_MODULES_RENDERABLE_SCENE_SRC_RENDERABLESCENE_HPP
