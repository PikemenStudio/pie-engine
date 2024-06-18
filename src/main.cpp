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
std::shared_ptr<BaseObject> Obj1;
std::vector<std::shared_ptr<BaseObject>> Objs;
std::shared_ptr<SceneManagerFacadeType> SceneManagerInstance1;

struct LocalObjectData {
  std::string Name;
  std::string TextureName;
  std::string ShaderSetName;
  std::string DumpName;
  std::string Type;
  std::string Path;
};
std::shared_ptr<BaseObject> createObject(LocalObjectData Data) {
  std::shared_ptr<BaseObject> Object =
      std::shared_ptr<BaseObject>(new Actor(Data.Path));
  Object->setName(Data.Name);
  Object->setTextureName(Data.TextureName);
  Object->setShaderSetName(Data.ShaderSetName);
  Object->setDumpName(Data.DumpName);
  Object->setType(Data.Type);
}

auto addObjects() {
  std::vector<std::shared_ptr<BaseObject>> Objects;

  std::vector<std::vector<bool>> TetrisScene{};
  for (size_t Width = 0; Width < 1; ++Width) {
    for (size_t Height = 0; Height < 1; ++Height) {
      for (size_t Depth = 0; Depth < 3; ++Depth) {
        std::shared_ptr<BaseObject> Object = std::shared_ptr<BaseObject>(
            new Actor("/Users/fullhat/Documents/chess.obj"));
        Object->setName(std::string("Actor") + std::to_string(Width) +
                        std::to_string(Height) + std::to_string(Depth));
        if (Depth == 0) {
          Object->setTextureName("Texture");
          Object->setDumpName("TS");
        } else {
          Object->setTextureName("Texture");
          Object->setDumpName("TS");
        }
        Object->setShaderSetName("default");
        Object->setType("Actor");
        Object->moveBy({2 * Width, 2 * Depth, 2 * Height});
        Objects.push_back(Object);
        Objs.push_back(Object);
      }
    }
  }
  Obj = Objects[0];

//  std::shared_ptr<BaseObject> Object = std::shared_ptr<BaseObject>(
//      new Actor("/Users/fullhat/Documents/chessboard.obj"));
//  Object->setName("Light");
//  Object->setTextureName("Texture");
//  Object->setDumpName("TS1");
//  Object->setShaderSetName("default");
//  Object->setType("Light");
//  Objects.push_back(Object);
//  Obj1 = Object;
  return Objects;
}
void moveObject() {
  while (true) {
    static uint64_t I = 0.0f;
    //    Obj->rotateBy({0.0f, 1.0f, 0.0f}, (float)I / 10000);
    I += 1;

    auto Camera = SceneManagerInstance1->ImplInstance.getCamera({0.0f, 0.0f});

    // Move camera in circle
    auto NewCameraPosition = glm::vec3(
        20.0f * std::cos(I / 10000000.0f), 20.0f * std::sin(I / 10000000.0f),
        std::abs(10.0f * std::sin(I / 10000000.0f)) + 10);
    SceneManagerInstance1->ImplInstance.setCamera(NewCameraPosition,
                                                  {0.0f, 0.0f, 0.0f});
//    SceneManagerInstance1->ImplInstance.setCamera({30.f, 30.f, 30.0f},
//                                                  {0.0f, 0.0f, 0.0f});

    //    Objs[0]->moveBy({0.0f, 0.0f, 0.0f});
    //    glm::vec3 EarthPos = glm::vec3(10.0f * std::cos(I / 1000000.0f),
    //                                   10.0f * std::sin(I / 1000000.0f),
    //                                   0.0f);
    //    Objs[1]->moveBy(EarthPos);
    //    Objs[2]->moveBy(glm::vec3(1.0f * std::cos(I / 100000.0f),
    //                              1.0f * std::sin(I / 100000.0f),
    //                              0.5f * std::sin(I / 100000.0f)) +
    //                    EarthPos);
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

    // Objects
    GraphicFacadeStructs::ObjectsData ObjectsData;
    Actor *A = (Actor *)Obj.get();
    ObjectsData["Actor"] = GraphicFacadeStructs::ObjectData{
        .Vertices = A->getVertices(),
        .Colors = A->getColors(),
        .TexCoords = A->getTextureCoords(),
        .Indexes = A->getIndexes(),
        .Normals = A->getNormals(),
    };

    GraphicAdapterInstance->ImplInstance.addObjectData(ObjectsData, "TS");

//    ObjectsData.clear();
//    Actor *B = (Actor *)Obj1.get();
//    ObjectsData["Light"] = GraphicFacadeStructs::ObjectData{
//        .Vertices = B->getVertices(),
//        .Colors = B->getColors(),
//        .TexCoords = B->getTextureCoords(),
//        .Indexes = B->getIndexes(),
//        .Normals = B->getNormals(),
//    };
//    GraphicAdapterInstance->ImplInstance.addObjectData(ObjectsData, "TS1");

    // Shader Sets
    GraphicAdapterInstance->ImplInstance.addShaderSet(
        "/Users/fullhat/Documents/GitHub/pie-engine/tests/resources/test1.vert",
        "/Users/fullhat/Documents/GitHub/pie-engine/tests/resources/test1.frag",
        "light");

    // Textures
    GraphicAdapterInstance->ImplInstance.addTextureSet(
        {"Texture",
         {"/Users/fullhat/Documents/GitHub/pie-engine/src/modules/graphics/"
          "sources/wood.jpg",
          "/Users/fullhat/Documents/GitHub/pie-engine/src/modules/graphics/"
          "sources/wood.jpg"}});
    GraphicAdapterInstance->ImplInstance.addTextureSet(
        {"Sun",
         {"/Users/fullhat/Documents/GitHub/pie-engine/src/modules/graphics/"
          "sources/preview_sun.jpg",
          "/Users/fullhat/Documents/GitHub/pie-engine/src/modules/graphics/"
          "sources/texture.jpg"}});
    GraphicAdapterInstance->ImplInstance.addTextureSet(
        {"Earth",
         {"/Users/fullhat/Documents/GitHub/pie-engine/src/modules/graphics/"
          "sources/2k_earth_daymap.jpg",
          "/Users/fullhat/Documents/GitHub/pie-engine/src/modules/graphics/"
          "sources/texture.jpg"}});
    GraphicAdapterInstance->ImplInstance.addTextureSet(
        {"Moon",
         {"/Users/fullhat/Documents/GitHub/pie-engine/src/modules/graphics/"
          "sources/preview_moon.jpg",
          "/Users/fullhat/Documents/GitHub/pie-engine/src/modules/graphics/"
          "sources/texture.jpg"}});
    GraphicAdapterInstance->ImplInstance.addTextureSet(
        {"ChessBoard",
         {"/Users/fullhat/Documents/GitHub/pie-engine/src/modules/graphics/"
          "sources/Unknown.jpeg",
          "/Users/fullhat/Documents/GitHub/pie-engine/src/modules/graphics/"
          "sources/texture.jpg"}});

    // Add light
    SceneManagerInstance->ImplInstance.addPointLight(
        std::shared_ptr<PointLight>(new PointLight(PointLight::PointLightProps{
            glm::vec4(-5.0f, -5.0f, 0.0f, 0.0f),
            glm::vec4(1.0f, 0.9f, 0.9f, 0.0f), 10.0f, "qwd1"})));

    SceneManagerInstance->ImplInstance.addPointLight(
        std::shared_ptr<PointLight>(new PointLight(PointLight::PointLightProps{
            glm::vec4(5.0f, 5.0f, 5.0f, 0.0f),
            glm::vec4(1.0f, 0.9f, 0.9f, 0.0f), 10.0f, "qwd"})));

    // Input
    InputManager::NativeType =
        (GLFWwindow *)WindowAdapterInstance->ImplInstance.getNativeType();
    InputManager::bindAction(GLFW_KEY_ESCAPE, [&]() {
      glfwSetWindowShouldClose(
          (GLFWwindow *)WindowAdapterInstance->ImplInstance.getNativeType(),
          GLFW_TRUE);
    });
    InputManager::bindMouseXY(
        [&](double X, double Y) { LOG_F(INFO, "Mouse X: %f, Y: %f", X, Y); });

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