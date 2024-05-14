//
// Created by FullHat on 18/04/2024.
//

#ifndef ENGINE_TESTS_GRAPHIC_MODULE_H
#define ENGINE_TESTS_GRAPHIC_MODULE_H

#include "../src/modules/shader_loader/facades/facade.hpp"
#include <graphics/facades/facade.hpp>
#include <gtest/gtest.h>
#include <iostream>

using WindowType = window_api_impls::WindowApiFacadeGlfwImpl;
using ShaderLoaderType = shader_loader_impls::ShaderLoaderSimpleImpl;

#define GraphicDependencies                                                    \
  graphic_api_impls::VulkanDependencies<WindowType, ShaderLoaderType>

class VkTest : public ::testing::Test {
protected:
  void SetUp() {
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

#ifdef __linux__
    std::string ShaderCompilerPath = "/usr/bin/glslc";
#else
    std::string ShaderCompilerPath = "/Users/fullhat/VulkanSDK/1.3.275.0/macOS/"
                                     "bin/glslc";
#endif // __linux__

    ASSERT_NO_THROW(
        ShaderLoaderInstance = std::shared_ptr<
            ShaderLoaderFacade<shader_loader_impls::ShaderLoaderSimpleImpl>>(
            new ShaderLoaderFacade<shader_loader_impls::ShaderLoaderSimpleImpl>(
                ShaderLoaderFacadeStructs::ShaderProps{
                    .CompilerPath = {ShaderCompilerPath},
                    .CacheFolder = {"~/Documents/"}
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
        GraphicFacadeStructs::GraphicEngineProps<GraphicDependencies>{
            .Dependencies = {WindowAdapterInstance, ShaderLoaderInstance},
            .InstancePropsInstance = InstanceProps,
            .PhysicalDevicePropsInstance = {},
        };

    GraphicAdapterInstance = new GraphicApiFacade<
        GraphicDependencies,
        graphic_api_impls::GraphicApiFacadeVulkanImpl<GraphicDependencies>>(
        std::move(FacadeProps));

    ASSERT_TRUE(GraphicAdapterInstance != nullptr);
  }

  void TearDown() { delete GraphicAdapterInstance; }

  GraphicApiFacade<
      GraphicDependencies,
      graphic_api_impls::GraphicApiFacadeVulkanImpl<GraphicDependencies>>
      *GraphicAdapterInstance;
};

TEST_F(VkTest, GraphicGetters) {
  ASSERT_TRUE(
      GraphicAdapterInstance->ImplInstance.getLocalPhysicalDevices().size() >
      0);
}

TEST_F(VkTest, GraphicSetupEngine) {
  ASSERT_NO_THROW(GraphicAdapterInstance->ImplInstance.chooseGpu(
      GraphicFacadeStructs::DeviceChoosePolicy::BEST));
}

TEST_F(VkTest, ChooseDefaultGpu) {
  ASSERT_NO_THROW(GraphicAdapterInstance->ImplInstance.chooseGpu({}));
}

#endif // ENGINE_TESTS_GRAPHIC_MODULE_H
