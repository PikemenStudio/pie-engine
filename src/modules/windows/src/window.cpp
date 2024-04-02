//
// Created by FullHat on 30/03/2024.
//

#include "window.h"

namespace windows {
Window::Window(Window::WindowProps Props) {}
Window::operator std::shared_ptr<GLFWwindow>() const { return GlfwWindow; }
void Window::buildWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, Props.IsResizable);

  auto *GlfwWindowPtr = glfwCreateWindow(Props.Size.y, Props.Size.x,
                                         Props.Title.c_str(), nullptr, nullptr);
  if (GlfwWindow == nullptr) {
    throw std::runtime_error("Failed to create GLFW window");
  }

  GlfwWindow =
      std::shared_ptr<GLFWwindow>(GlfwWindowPtr, [](GLFWwindow *WindowObj) {
        glfwDestroyWindow(WindowObj);
        glfwTerminate();
      });
}
} // namespace windows