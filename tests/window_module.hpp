//
// Created by FullHat on 18/04/2024.
//

#ifndef ENGINE_TESTS_WINDOW_MODULE_HPP
#define ENGINE_TESTS_WINDOW_MODULE_HPP

#include <gtest/gtest.h>
#include <graphics/facades/facade.hpp>

class GlfwTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    ASSERT_NO_THROW(WindowAdapterInstance =
                        new WindowApiFacade<>(WindowFacadeStructs::WindowProps{
                            .Size = {800, 600},
                            .Title = "Test",
                            .Mode = WindowFacadeStructs::WindowProps::WINDOWED,
                            .IsResizable = false,
                        }));
  }

  static void TearDownTestSuite() {
    delete WindowAdapterInstance;
  }

  static inline WindowApiFacade<> *WindowAdapterInstance;
};

// Test case to list all Vulkan devices
TEST_F(GlfwTest, WindowGetters) {
  void *Native = WindowAdapterInstance->ImplInstance.getNativeType();
  ASSERT_TRUE(Native != nullptr);

  // Glfw must have at least one required extension, maybe empty in other
  // libraries
  auto Extensions = WindowAdapterInstance->ImplInstance.getRequiredExtensions();
  ASSERT_TRUE(Extensions.size() > 0);
}

#endif // ENGINE_TESTS_WINDOW_MODULE_HPP
