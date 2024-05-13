//
// Created by FullHat on 30/03/2024.
//

#include "window.hpp"

namespace windows {
Window::Window(Window::WindowProps Props) : Props(Props) { buildWindow(); }

Window::operator std::shared_ptr<GLFWwindow>() const { return GlfwWindow; }

void Window::buildWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, Props.IsResizable ? GLFW_TRUE : GLFW_FALSE);

  auto *GlfwWindowPtr = glfwCreateWindow(Props.Size.y, Props.Size.x,
                                         Props.Title.c_str(), nullptr, nullptr);

  if (GlfwWindowPtr == nullptr) {
    throw std::runtime_error("Failed to create GLFW window");
  }

  GlfwWindow =
      std::shared_ptr<GLFWwindow>(GlfwWindowPtr, [](GLFWwindow *WindowObj) {
        glfwDestroyWindow(WindowObj);
        glfwTerminate();
      });
}
std::vector<const char *> Window::getRequiredExtensions() const {
  uint32_t Size;
  const auto *Extensions = glfwGetRequiredInstanceExtensions(&Size);
  return {Extensions, Extensions + Size};
}

std::pair<uint32_t, uint32_t> Window::getSize() const {
  return std::pair<uint32_t, uint32_t>(this->Props.Size.x, this->Props.Size.y);
}

void Window::prepare() {
  glfwMakeContextCurrent(GlfwWindow.get());
}

bool Window::shouldClose() const {
  return glfwWindowShouldClose(GlfwWindow.get());
}

void Window::update() {
  glfwPollEvents();
}

void Window::swapBuffers() { glfwSwapBuffers(GlfwWindow.get()); }

void Window::setTitle(const std::string &Title) {
  glfwSetWindowTitle(GlfwWindow.get(), Title.c_str());
}
} // namespace windows