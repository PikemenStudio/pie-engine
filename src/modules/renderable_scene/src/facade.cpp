//
// Created by FullHat on 13/05/2024.
//

#include "../facades/facade.hpp"
#include "RenderableScene.hpp"

using WindowType = window_api_impls::WindowApiFacadeGlfwImpl;
using ShaderLoaderType = shader_loader_impls::ShaderLoaderSimpleImpl;
using GraphicsType = graphic_api_impls::GraphicApiFacadeVulkanImpl<
    graphic_api_impls::VulkanDependencies<WindowType, ShaderLoaderType>>;
using SceneManagerType = scene_manager_facades::SceneManagerBaseImpl<scene_manager_facades::SceneManagerDependencies>;
using DependenciesType = renderable_scene_facades::RenderableSceneDependencies<
    scene_manager_facades::SceneManagerDependencies, SceneManagerType,
    graphic_api_impls::VulkanDependencies<WindowType, ShaderLoaderType>,
    GraphicsType, WindowType>;

using namespace renderable_scene_facades;

template <RenderableSceneDependenciesConcept DepT>
RenderableSceneBaseImpl<DepT>::RenderableSceneBaseImpl(
    RenderableSceneFacadeStructs::RenderableSceneProps<DepT> &&Props) {
  using NativeType = rs::RenderableScene<
      typename DepT::SceneManagerType::DepType, typename DepT::SceneManagerType,
      typename DepT::GraphicsType::DepType, typename DepT::GraphicsType, typename DepT::WindowType>;

  typename NativeType::RenderableSceneProps ConvertedProps = {
      .Dependencies = {
          .SceneManager = Props.Dependencies.SceneManager,
          .Graphics = Props.Dependencies.Graphics,
          .Window = Props.Dependencies.Window,
      }};

  Data = new NativeType(std::move(ConvertedProps));
}

template <RenderableSceneDependenciesConcept DepT>
RenderableSceneBaseImpl<DepT>::~RenderableSceneBaseImpl() {
  using NativeType = rs::RenderableScene<
      typename DepT::SceneManagerType::DepType, typename DepT::SceneManagerType,
      typename DepT::GraphicsType::DepType, typename DepT::GraphicsType, typename DepT::WindowType>;

  auto *NativeData = static_cast<NativeType *>(Data);

  delete NativeData;
}

template <RenderableSceneDependenciesConcept DepT>
void RenderableSceneBaseImpl<DepT>::runMainCycle() {
  using NativeType = rs::RenderableScene<
      typename DepT::SceneManagerType::DepType, typename DepT::SceneManagerType,
      typename DepT::GraphicsType::DepType, typename DepT::GraphicsType, typename DepT::WindowType>;

  auto *NativeData = static_cast<NativeType *>(Data);

  NativeData->runMainCycle();
}

template class renderable_scene_facades::RenderableSceneBaseImpl<
    DependenciesType>;
