#include <iostream>
//
// Created by FullHat on 28/03/2024.
//
#include <loguru.hpp>

#include "modules/vk_core/facades/facade.hpp"
#include "modules/windows/facades/facade.hpp"

int main(int Argc, char *Argv[]) {
  loguru::init(Argc, Argv);
  loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);

  WindowApiFacade<> WindowAdapter(WindowFacadeStructs::WindowProps{
      .Size = {800, 600},
      .Title = "Test",
      .Mode = WindowFacadeStructs::WindowProps::WINDOWED,
      .IsResizable = false,
  });

  auto InstanceProps = GraphicFacadeStructs::InstanceProps{
      .AppName = "Test",
      .EngineName = "Test",
      .AppVersion = {.Major = 1, .Minor = 0, .Patch = 0},
      .EngineVersion = {.Major = 1, .Minor = 0, .Patch = 0},
      .RequestedWindowExtensions = WindowAdapter.ImplInstance.getRequiredExtensions(),
  };

  auto a = GraphicFacadeStructs::GraphicEngineProps<window_api_impls::WindowApiFacadeGlfwImpl>{
      .Window = std::move(WindowAdapter),
      .VkInstanceProps = InstanceProps,
      .VkPhysicalDeviceProps = {},
  };

  GraphicApiFacade<window_api_impls::WindowApiFacadeGlfwImpl> Adapter(std::move(a));

  std::cout << "Found " << Adapter.ImplInstance.getLocalPhysicalDevices().size()
            << " devices" << std::endl;

  // Choose default gpu
  Adapter.ImplInstance.chooseGpu(GraphicFacadeStructs::DeviceChoosePolicy::BEST, true);

  std::cout << "Hello world" << std::endl;
  return 0;
}