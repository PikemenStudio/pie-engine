//
// Created by FullHat on 14/04/2024.
//

#include "../facades/facade.hpp"
#include "window.hpp"

using namespace window_api_impls;

using GlfwImpl = window_api_impls::WindowApiFacadeGlfwImpl;

using DataType = windows::Window;
using DataTypePtr = DataType *;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convertors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

windows::Window::WindowProps toModuleType(WindowFacadeStructs::WindowProps &&Props) {
  windows::Window::WindowProps ModuleProps{
      .Size = Props.Size,
      .Title = std::move(Props.Title),
  };
  return ModuleProps;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Implementation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

GlfwImpl::WindowApiFacadeGlfwImpl(WindowFacadeStructs::WindowProps &&Props) {
  Data = new windows::Window(toModuleType(std::move(Props)));
}

GlfwImpl::~WindowApiFacadeGlfwImpl() {
  if (Data == nullptr) {
    return;
  }
  delete static_cast<DataTypePtr>(Data);
  Data = nullptr;
}

void* GlfwImpl::getNativeType() const {
  return static_cast<DataTypePtr>(Data)->getNativeWindow();
}

std::vector<const char *> GlfwImpl::getRequiredExtensions() const {
  return static_cast<DataTypePtr>(Data)->getRequiredExtensions();
}
