//
// Created by FullHat on 10/05/2024.
//

#include "../facades/facade.hpp"
#include <fstream>
#include <loguru.hpp>

#define DataType ShaderLoaderFacadeStructs::ShaderProps;

shader_loader_impls::ShaderLoaderSimpleImpl::ShaderLoaderSimpleImpl(
    ShaderLoaderFacadeStructs::ShaderProps &&Props) {
  Data = new ShaderLoaderFacadeStructs::ShaderProps(std::move(Props));
}

shader_loader_impls::ShaderLoaderSimpleImpl::~ShaderLoaderSimpleImpl() {
  delete static_cast<ShaderLoaderFacadeStructs::ShaderProps *>(Data);
}

std::vector<ShaderLoaderFacadeStructs::ShaderData>
shader_loader_impls::ShaderLoaderSimpleImpl::loadAndCompileShaders(
    const ShaderLoaderFacadeStructs::ShadersLocations &Locations) {
  std::vector<ShaderLoaderFacadeStructs::ShaderData> Result;

  for (const auto &Name : Locations.Shaders) {
    LOG_F(INFO, "Loading shader: %s", Name.ShaderName.c_str());

    // Load shader from file
    std::ifstream ShaderFile(Name.ShaderPath);
    if (!ShaderFile.is_open()) {
      LOG_F(ERROR, "Failed to open shader file: %s", Name.ShaderPath.c_str());
      continue;
    }

    std::stringstream ShaderData;
    ShaderData << ShaderFile.rdbuf();
    LOG_F(INFO, "Shader loaded: %s", (ShaderData.str().substr(0, 20) + "...").c_str());

    // Compile shader
    std::string CompilerPath =
        (*(static_cast<ShaderLoaderFacadeStructs::ShaderProps *>(Data)))
            .CompilerPath.value();
    std::string CachePath =
        (*(static_cast<ShaderLoaderFacadeStructs::ShaderProps *>(Data)))
            .CacheFolder.value();
    std::string Command = CompilerPath + " " + Name.ShaderPath + " -o " +
                          CachePath + Name.ShaderName + ".spv";
    LOG_F(INFO, "Compiling shader: %s", Command.c_str());
    auto Code = std::system(Command.c_str());

    if (Code != 0) {
      LOG_F(ERROR, "Failed to compile shader: %s", Name.ShaderName.c_str());
      continue;
    }

    std::ifstream ShaderResult(Name.ShaderName + ".spv");
    Result.emplace_back(ShaderLoaderFacadeStructs::ShaderData{
        .Name = Name.ShaderName, .Data = std::move(ShaderData)});
  }

  return Result;
}

// Explicitly instantiate class
template class ShaderLoaderFacade<shader_loader_impls::ShaderLoaderSimpleImpl>;