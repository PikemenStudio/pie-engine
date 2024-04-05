#include <iostream>
//
// Created by FullHat on 28/03/2024.
//
#include <loguru.hpp>

#include "modules/vk_core/adapters/fabric.hpp"
#include "modules/windows/adapters/fabric.hpp"

int main(int Argc, char *Argv[]) {
  loguru::init(Argc, Argv);
  loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);

  WindowAdapter WindowAdapter(windows::Window::WindowProps{
      .Size = {800, 600},
      .Title = "Test",
      .Mode = windows::Window::WindowProps::WINDOWED,
      .IsResizable = true,
  });

  auto InstanceProps = vk_core::VkInstance::VkInstanceProps{
      .AppName = "Test",
      .EngineName = "Test",
      .AppVersion = {.Major = 1, .Minor = 0, .Patch = 0},
      .EngineVersion = {.Major = 1, .Minor = 0, .Patch = 0},
      .RequestedWindowExtensions = WindowAdapter.getRequiredExtensions(),
  };

  GraphicApiAdapter Adapter(vk_core::GraphicEngine::GraphicEngineProps{
      .Window = std::move(WindowAdapter),
      .VkInstanceProps = InstanceProps,
      .VkPhysicalDeviceProps = {},
  });

  std::cout << "Hello world" << std::endl;
  return 0;
}