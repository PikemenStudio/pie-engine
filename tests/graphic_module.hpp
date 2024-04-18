//
// Created by FullHat on 18/04/2024.
//

#ifndef ENGINE_TESTS_GRAPHIC_MODULE_H
#define ENGINE_TESTS_GRAPHIC_MODULE_H

#include <graphics/facades/facade.hpp>
#include <gtest/gtest.h>
#include <windows/facades/facade.hpp>

class VkTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    ASSERT_NO_THROW(WindowAdapterInstance =
                        new WindowApiFacade<>(WindowFacadeStructs::WindowProps{
                            .Size = {800, 600},
                            .Title = "Test",
                            .Mode = WindowFacadeStructs::WindowProps::WINDOWED,
                            .IsResizable = false,
                        }));

    auto InstanceProps = GraphicFacadeStructs::InstanceProps{
        .AppName = "Test",
        .EngineName = "Test",
        .AppVersion = {.Major = 1, .Minor = 0, .Patch = 0},
        .EngineVersion = {.Major = 1, .Minor = 0, .Patch = 0},
        .RequestedWindowExtensions =
            WindowAdapterInstance->ImplInstance.getRequiredExtensions(),
    };

    auto FacadeProps = GraphicFacadeStructs::GraphicEngineProps<
        window_api_impls::WindowApiFacadeGlfwImpl>{
        .Window = std::move(*WindowAdapterInstance),
        .InstanceProps = InstanceProps,
        .PhysicalDeviceProps = {},
    };

    ASSERT_NO_THROW(
        GraphicAdapterInstance =
            new GraphicApiFacade<window_api_impls::WindowApiFacadeGlfwImpl>(
                std::move(FacadeProps)));
  }

  static void TearDownTestSuite() {}

  static inline WindowApiFacade<> *WindowAdapterInstance;
  static inline GraphicApiFacade<window_api_impls::WindowApiFacadeGlfwImpl>
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

  ASSERT_NO_THROW(GraphicAdapterInstance->ImplInstance.chooseGpu({}));
}
#endif // ENGINE_TESTS_GRAPHIC_MODULE_H
