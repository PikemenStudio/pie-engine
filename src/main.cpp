#include <iostream>
//
// Created by FullHat on 28/03/2024.
//
#include <loguru.hpp>

#include "modules/graphics/facades/facade.hpp"
#include "modules/shader_loader/facades/facade.hpp"
#include "modules/windows/facades/facade.hpp"

#include "../src/modules/graphics/facades/facade.hpp"
#include "../src/modules/renderable_scene//facades/facade.hpp"
#include "../src/modules/scene_manager/facades/facade.hpp"
#include <GLFW/glfw3.h>

#include <gtest/gtest.h>
#include <iostream>

using WindowType = window_api_impls::WindowApiFacadeGlfwImpl;
using ShaderLoaderType = shader_loader_impls::ShaderLoaderSimpleImpl;

using SceneManagerDependencies =
    scene_manager_facades::SceneManagerDependencies;
using SceneManagerClass =
    scene_manager_facades::SceneManagerBaseImpl<SceneManagerDependencies>;

using SceneManagerFacadeType =
    SceneManagerFacade<SceneManagerDependencies, SceneManagerClass>;

using GraphicDependenciesType =
    graphic_api_impls::VulkanDependencies<WindowType, ShaderLoaderType>;
using GraphicClass =
    graphic_api_impls::GraphicApiFacadeVulkanImpl<GraphicDependenciesType>;

using RenderableSceneDependencies =
    renderable_scene_facades::RenderableSceneDependencies<
        SceneManagerDependencies, SceneManagerClass, GraphicDependenciesType,
        GraphicClass, WindowType>;

using RenderableSceneFacadeType =
    RenderableSceneFacade<RenderableSceneDependencies,
                          renderable_scene_facades::RenderableSceneBaseImpl<
                              RenderableSceneDependencies>>;

using GraphicApiFacadeType = GraphicApiFacade<
    GraphicDependenciesType,
    graphic_api_impls::GraphicApiFacadeVulkanImpl<GraphicDependenciesType>>;

class RenderableSceneTest {
public:
  void SetUp() {
    // Setup Graphic
    std::shared_ptr<WindowApiFacade<>> WindowAdapterInstance;
    WindowAdapterInstance = std::shared_ptr<WindowApiFacade<>>(
        new WindowApiFacade<>(WindowFacadeStructs::WindowProps{
            .Size = {800, 600},
            .Title = "Test",
            .Mode = WindowFacadeStructs::WindowProps::WINDOWED,
            .IsResizable = false,
        }));

#ifdef __linux__
    std::string ShaderCompilerPath = "/usr/bin/glslc";
#else
    std::string ShaderCompilerPath = "/Users/fullhat/VulkanSDK/1.3.275.0/macOS/"
                                     "bin/glslc";
#endif // __linux__

    std::shared_ptr<
        ShaderLoaderFacade<shader_loader_impls::ShaderLoaderSimpleImpl>>
        ShaderLoaderInstance;
    ShaderLoaderInstance = std::shared_ptr<
        ShaderLoaderFacade<shader_loader_impls::ShaderLoaderSimpleImpl>>(
        new ShaderLoaderFacade<shader_loader_impls::ShaderLoaderSimpleImpl>(
            ShaderLoaderFacadeStructs::ShaderProps{
                .CompilerPath = {ShaderCompilerPath},
                .CacheFolder = {"~/Documents/"},
            }));

    auto InstanceProps = GraphicFacadeStructs::InstanceProps{
        .AppName = "Test",
        .EngineName = "Test",
        .AppVersion = {.Major = 1, .Minor = 0, .Patch = 0},
        .EngineVersion = {.Major = 1, .Minor = 0, .Patch = 0},
        .RequestedWindowExtensions =
            // clang-format off
            WindowAdapterInstance->ImplInstance.getRequiredExtensions(),
        // clang-format on
    };

    auto FacadeProps =
        GraphicFacadeStructs::GraphicEngineProps<GraphicDependenciesType>{
            .Dependencies = {WindowAdapterInstance, ShaderLoaderInstance},
            .InstancePropsInstance = InstanceProps,
            .PhysicalDevicePropsInstance = {},
        };

    GraphicAdapterInstance = std::shared_ptr<GraphicApiFacadeType>(
        new GraphicApiFacade<GraphicDependenciesType,
                             graphic_api_impls::GraphicApiFacadeVulkanImpl<
                                 GraphicDependenciesType>>(
            std::move(FacadeProps)));

    std::shared_ptr<SceneManagerFacadeType> SceneManagerInstance =
        std::make_shared<SceneManagerFacadeType>(
            SceneManagerFacadeStructs::SceneManagerProps<
                SceneManagerDependencies>{.Dependencies = {}});

    RenderableSceneInstance = std::make_shared<RenderableSceneFacadeType>(
        RenderableSceneFacadeStructs::RenderableSceneProps<
            RenderableSceneDependencies>{
            .Dependencies = {.SceneManager = SceneManagerInstance,
                             .Graphics = GraphicAdapterInstance,
                             .Window = WindowAdapterInstance}});

    GraphicAdapterInstance->ImplInstance.chooseGpu({});

    RenderableSceneInstance->ImplInstance.runMainCycle();
  }

  void TearDown() {}

  std::shared_ptr<GraphicApiFacadeType> GraphicAdapterInstance;

  std::shared_ptr<RenderableSceneFacadeType> RenderableSceneInstance;
};

int main(int Argc, char *Argv[]) {
  loguru::init(Argc, Argv);
  // clang-format off
  loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);
  // clang-format on

  RenderableSceneTest Test;
  Test.SetUp();
  Test.TearDown();

  return 0;
}