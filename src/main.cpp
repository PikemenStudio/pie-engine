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
    graphic_api_impls::VulkanDependencies<WindowType, ShaderLoaderType,
                                          SceneManagerClass>;
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
    std::runtime_error err("");
    err.what();
    // Setup Graphic
    std::shared_ptr<WindowApiFacade<>> WindowAdapterInstance;
    WindowAdapterInstance = std::shared_ptr<WindowApiFacade<>>(
        new WindowApiFacade<>(WindowFacadeStructs::WindowProps{
            .Size = {800, 600},
            .Title = "Test",
            .Mode = WindowFacadeStructs::WindowProps::WINDOWED,
            .IsResizable = true,
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

    std::shared_ptr<SceneManagerFacadeType> SceneManagerInstance =
        std::make_shared<SceneManagerFacadeType>(
            SceneManagerFacadeStructs::SceneManagerProps<
                SceneManagerDependencies>{.Dependencies = {}});
    std::unique_ptr<BaseObject> Object = std::unique_ptr<BaseObject>(
        new Triangle({glm::vec3(), glm::vec3(), glm::vec3()}));
    Object->moveBy({0.5, 0.5, 0.5});
    std::unique_ptr<BaseObject> Object1 = std::unique_ptr<BaseObject>(
        new Triangle({glm::vec3(), glm::vec3(), glm::vec3()}));
    std::unique_ptr<BaseObject> Object2 = std::unique_ptr<BaseObject>(
        new Square({glm::vec3(), glm::vec3(), glm::vec3(), glm::vec3()}));
    SceneManagerInstance->ImplInstance.addObject(std::move(Object));
    SceneManagerInstance->ImplInstance.addObject(std::move(Object1));
    SceneManagerInstance->ImplInstance.addObject(std::move(Object2));

    auto FacadeProps =
        GraphicFacadeStructs::GraphicEngineProps<GraphicDependenciesType>{
            .Dependencies =
                graphic_api_impls::VulkanDependencies<
                    WindowType, ShaderLoaderType, SceneManagerClass>{
                    .Window = WindowAdapterInstance,
                    .ShaderLoader = ShaderLoaderInstance,
                    .SceneManager = SceneManagerInstance,
                },
            .InstancePropsInstance = InstanceProps,
            .PhysicalDevicePropsInstance = {},
        };

    GraphicAdapterInstance = std::shared_ptr<GraphicApiFacadeType>(
        new GraphicApiFacade<GraphicDependenciesType,
                             graphic_api_impls::GraphicApiFacadeVulkanImpl<
                                 GraphicDependenciesType>>(
            std::move(FacadeProps)));

    RenderableSceneInstance = std::make_shared<RenderableSceneFacadeType>(
        RenderableSceneFacadeStructs::RenderableSceneProps<
            RenderableSceneDependencies>{
            .Dependencies = {.SceneManager = SceneManagerInstance,
                             .Graphics = GraphicAdapterInstance,
                             .Window = WindowAdapterInstance}});

    GraphicAdapterInstance->ImplInstance.chooseGpu({});

    GraphicFacadeStructs::ObjectsData ObjectsData;
    ObjectsData["Triangle"] = GraphicFacadeStructs::ObjectData{
        .Vertices = {0.0f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f},
        .Colors = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    GraphicFacadeStructs::ObjectsData ObjectsData1;
    ObjectsData1["Square"] = GraphicFacadeStructs::ObjectData{
        .Vertices = {-0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5, 0.5},
        .Colors = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                   1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}};
    GraphicAdapterInstance->ImplInstance.addObjectData(ObjectsData, "TS");
    GraphicAdapterInstance->ImplInstance.addObjectData(ObjectsData1, "TS1");

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