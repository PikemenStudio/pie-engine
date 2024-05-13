//
// Created by FullHat on 13/05/2024.
//

#ifndef ENGINE_TESTS_RENDERABLE_SCENE_HPP
#define ENGINE_TESTS_RENDERABLE_SCENE_HPP

#include "../src/modules/graphics/facades/facade.hpp"
#include "../src/modules/renderable_scene//facades/facade.hpp"
#include "../src/modules/scene_manager/facades/facade.hpp"
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

class RenderableSceneTest : public ::testing::Test {
public:
  void SetUp() override {
    // Setup Graphic
    std::shared_ptr<WindowApiFacade<>> WindowAdapterInstance;
    ASSERT_NO_THROW(WindowAdapterInstance = std::shared_ptr<WindowApiFacade<>>(
                        new WindowApiFacade<>(WindowFacadeStructs::WindowProps{
                            .Size = {800, 600},
                            .Title = "Test",
                            .Mode = WindowFacadeStructs::WindowProps::WINDOWED,
                            .IsResizable = false,
                        })));

    std::shared_ptr<
        ShaderLoaderFacade<shader_loader_impls::ShaderLoaderSimpleImpl>>
        ShaderLoaderInstance;
    ASSERT_NO_THROW(
        ShaderLoaderInstance = std::shared_ptr<
            ShaderLoaderFacade<shader_loader_impls::ShaderLoaderSimpleImpl>>(
            new ShaderLoaderFacade<shader_loader_impls::ShaderLoaderSimpleImpl>(
                ShaderLoaderFacadeStructs::ShaderProps{
                    .CompilerPath = {"/Users/fullhat/VulkanSDK/1.3.275.0/macOS/"
                                     "bin/glslc"},
                    .CacheFolder = {"~/Documents/"},
                })));

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

    ASSERT_TRUE(GraphicAdapterInstance != nullptr);

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
  }

  void TearDown() override {}

  std::shared_ptr<GraphicApiFacadeType> GraphicAdapterInstance;

  std::shared_ptr<RenderableSceneFacadeType> RenderableSceneInstance;
};

TEST_F(RenderableSceneTest, TestRenderableScene) {
  GraphicAdapterInstance->ImplInstance.chooseGpu({});

  RenderableSceneInstance->ImplInstance.runMainCycle();
}

#endif // ENGINE_TESTS_RENDERABLE_SCENE_HPP
