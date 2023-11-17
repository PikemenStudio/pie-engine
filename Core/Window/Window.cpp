#pragma once

#include "Window.hpp"

Window::Window(const glm::uvec2 &_size, const std::string &_title)
    : m_size(_size), m_title(_title) {}

Window::Window() : m_size(0, 0) {}

Window::~Window() { glfwTerminate(); }

void Window::BuildGlfwWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  m_window =
      glfwCreateWindow(m_size.x, m_size.y, m_title.c_str(), nullptr, nullptr);

  if (!m_window) {
    throw std::runtime_error("Can't setup monitor");
  }
}

void Window::Start() { 
    glfwMakeContextCurrent(m_window); }

void Window::ClearBuffer() {  }

void Window::SwapBuffers() { glfwSwapBuffers(m_window); }

void Window::PollEvents() { glfwPollEvents(); }

bool Window::IsShouldClose() const { return glfwWindowShouldClose(m_window); }
