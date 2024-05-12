//
// Created by FullHat on 11/05/2024.
//

#ifndef ENGINE_TESTS_SHADER_LOADER_MODULE_H
#define ENGINE_TESTS_SHADER_LOADER_MODULE_H

#include "../src/modules/shader_loader/facades/facade.hpp"

#include <gtest/gtest.h>

class ShaderLoaderTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    auto Props = ShaderLoaderFacadeStructs::ShaderProps{
        .CompilerPath =
            std::optional<std::string>{
                "/Users/fullhat/VulkanSDK/1.3.275.0/macOS/bin/glslc"},
        .CacheFolder = std::optional<std::string>{"~/Documents"},
    };

    ASSERT_NO_THROW(
        ShaderLoaderAdapterInstance =
            new ShaderLoaderFacade<shader_loader_impls::ShaderLoaderSimpleImpl>(
                std::move(Props)));

    // ASSERT_TRUE(ShaderLoaderAdapterInstance != nullptr);
  }

  static void TearDownTestSuite() {}

  static inline ShaderLoaderFacade<shader_loader_impls::ShaderLoaderSimpleImpl>
      *ShaderLoaderAdapterInstance = nullptr;
};

TEST_F(ShaderLoaderTest, LoadAndCompileShaders) {
  ShaderLoaderFacadeStructs::ShadersLocations Locations;
  Locations.Shaders.push_back({
      .ShaderName = "test",
      .ShaderPath = "/Users/fullhat/Documents/GitHub/pie-engine/tests/"
                    "resources/test.vert",
  });

  auto Result = ShaderLoaderAdapterInstance->ImplInstance.loadAndCompileShaders(
      Locations);

  ASSERT_TRUE(Result[0].Name == "test");
  ASSERT_TRUE(Result[0].Data.str().size() > 0);
}

#endif // ENGINE_TESTS_SHADER_LOADER_MODULE_H
