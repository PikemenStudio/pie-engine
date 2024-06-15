#include <iostream>
//
// Created by FullHat on 28/03/2024.
//
#include <loguru.hpp>

#include "modules/graphics/facades/facade.hpp"
#include "modules/shader_loader/facades/facade.hpp"
#include "modules/windows/facades/facade.hpp"

#include "../src/modules/graphics/facades/facade.hpp"
#include "../src/modules/input_manager/InputManager.hpp"
#include "../src/modules/renderable_scene//facades/facade.hpp"
#include "../src/modules/scene_manager/facades/facade.hpp"
#include <GLFW/glfw3.h>

#include <gtest/gtest.h>
#include <iostream>
#include <thread>

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

std::shared_ptr<BaseObject> Obj;
std::shared_ptr<SceneManagerFacadeType> SceneManagerInstance1;

auto addObjects() {
  std::vector<std::shared_ptr<BaseObject>> Objects;

  std::vector<std::vector<bool>> TetrisScene{};
  for (size_t Width = 0; Width < 1; ++Width) {
    for (size_t Height = 0; Height < 2; ++Height) {
      for (size_t Depth = 0; Depth < 2; ++Depth) {
        std::shared_ptr<BaseObject> Object = std::shared_ptr<BaseObject>(
            new Actor("/Users/fullhat/Documents/tetrice_cube_no_mods.obj"));
        Object->setName(std::string("Actor") + std::to_string(Width) +
                        std::to_string(Height) + std::to_string(Depth));
        Object->setTextureName("Texture");
        Object->setDumpName("TS");
        if (Height == 0) {
          Object->setShaderSetName("default");
        } else {
          Object->setShaderSetName("a");
        }
        Object->moveBy({4 * Width, 4 * Depth, 4 * Height});
        Objects.push_back(Object);
      }
    }
  }
  Obj = Objects[0];
  return Objects;
}
void moveObject() {
  while (true) {
    static uint64_t I = 0.0f;
    // Obj->rotateBy({0.0f, 1.0f, 0.0f}, (float)I / 10000);
    I += 1;
  }
}

class RenderableSceneTest {
public:
  void SetUp() {
    std::runtime_error err("");
    err.what();
    // Setup Graphic
    std::shared_ptr<WindowApiFacade<>> WindowAdapterInstance;
    WindowAdapterInstance = std::shared_ptr<WindowApiFacade<>>(
        new WindowApiFacade<>(WindowFacadeStructs::WindowProps{
            .Size = {3456, 2234},
            .Title = "Test",
            .Mode = WindowFacadeStructs::WindowProps::FULLSCREEN,
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
    SceneManagerInstance1 = SceneManagerInstance;

    for (auto Object : addObjects()) {
      SceneManagerInstance->ImplInstance.addObject(Object);
    }

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
    Actor *A = (Actor *)Obj.get();
    ObjectsData["Actor"] = GraphicFacadeStructs::ObjectData{
        .Vertices = A->getVertices(),
        .Colors = A->getColors(),
        .TexCoords = A->getTextureCoords(),
        .Indexes = A->getIndexes(),
        .Normals = A->getNormals(),
    };

    GraphicFacadeStructs::ObjectsData ObjectsData1;
    Actor *A1 = (Actor *)Obj.get();
    ObjectsData1["Actor"] = GraphicFacadeStructs::ObjectData{
        .Vertices = A1->getVertices(),
        .Colors = A1->getColors(),
        .TexCoords = A->getTextureCoords(),
        .Indexes = A->getIndexes(),
        .Normals = A->getNormals(),
    };
    GraphicAdapterInstance->ImplInstance.addObjectData(ObjectsData, "TS");
    GraphicAdapterInstance->ImplInstance.addObjectData(ObjectsData1, "TS1");

    GraphicAdapterInstance->ImplInstance.addTexture(
        "/Users/fullhat/Documents/GitHub/pie-engine/src/modules/graphics/"
        "sources/texture.jpg",
        "Texture");
    GraphicAdapterInstance->ImplInstance.addTexture(
        "/Users/fullhat/Documents/GitHub/pie-engine/src/modules/graphics/"
        "sources/OIG3.JiRSM54Q19NgBbSeHmTz.jpeg",
        "Texture1");

    GraphicAdapterInstance->ImplInstance.addShaderSet(
        "/Users/fullhat/Documents/GitHub/pie-engine/tests/"
        "resources/test1.vert",
        "/Users/fullhat/Documents/GitHub/pie-engine/tests/"
        "resources/test1.frag",
        "a");

    InputManager::NativeType =
        (GLFWwindow *)WindowAdapterInstance->ImplInstance.getNativeType();
    InputManager::bindAction(GLFW_KEY_ESCAPE, [&]() {
      glfwSetWindowShouldClose(
          (GLFWwindow *)WindowAdapterInstance->ImplInstance.getNativeType(),
          GLFW_TRUE);
    });
    InputManager::bindMouseXY([&](double X, double Y) {
      LOG_F(INFO, "Mouse X: %f, Y: %f", X, Y);
    });

    std::thread Thread(moveObject);
    Thread.detach();
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