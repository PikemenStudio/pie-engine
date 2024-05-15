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

windows::Window::WindowProps
toModuleType(WindowFacadeStructs::WindowProps &&Props) {
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

void *GlfwImpl::getNativeType() const {
  return static_cast<DataTypePtr>(Data)->getNativeWindow();
}

std::vector<const char *> GlfwImpl::getRequiredExtensions() const {
  return static_cast<DataTypePtr>(Data)->getRequiredExtensions();
}

std::pair<uint32_t, uint32_t> GlfwImpl::getSize() const {
  return static_cast<DataTypePtr>(Data)->getSize();
}

void GlfwImpl::setSize(const std::pair<uint32_t, uint32_t> &Size) const {
  static_cast<DataTypePtr>(Data)->setSize(Size);
}

void GlfwImpl::prepare() { static_cast<DataTypePtr>(Data)->prepare(); }

bool GlfwImpl::shouldClose() {
  return static_cast<DataTypePtr>(Data)->shouldClose();
}

void GlfwImpl::update() { static_cast<DataTypePtr>(Data)->update(); }

void GlfwImpl::swapBuffers() { static_cast<DataTypePtr>(Data)->swapBuffers(); }

void GlfwImpl::setTitle(const std::string &Title) {
  static_cast<DataTypePtr>(Data)->setTitle(Title);
}
