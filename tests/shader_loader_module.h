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
#ifdef __linux__
    std::string ShaderCompilerPath = "/usr/bin/glslc";
#else
    std::string ShaderCompilerPath = "/Users/fullhat/VulkanSDK/1.3.275.0/macOS/"
                                     "bin/glslc";
#endif // __linux__

    auto Props = ShaderLoaderFacadeStructs::ShaderProps{
        .CompilerPath = std::optional<std::string>{ShaderCompilerPath},
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

#ifdef __linux__
  std::string ShaderPath = "/home/anton/dev/engine/pie-engine/tests/resources/test.vert";
#else
  std::string ShaderPath = "/Users/fullhat/Documents/GitHub/pie-engine/tests/"
                           "resources/test.vert";
#endif

  Locations.Shaders.push_back({
      .ShaderName = "test",
      .ShaderPath = ShaderPath,
  });

  auto Result = ShaderLoaderAdapterInstance->ImplInstance.loadAndCompileShaders(
      Locations);

  ASSERT_TRUE(Result[0].Name == "test");
  ASSERT_TRUE(Result[0].Data.str().size() > 0);
}

#endif // ENGINE_TESTS_SHADER_LOADER_MODULE_H
