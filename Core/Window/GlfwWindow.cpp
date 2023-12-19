#include "GlfwWindow.hpp"

#include <utility>

GlfwWindow::GlfwWindow() : m_size(0, 0) {
    BuildGlfwWindow();
}

GlfwWindow::GlfwWindow(const glm::uvec2 &_size, std::string _title)
        : m_size(_size), m_title(_title) {
    BuildGlfwWindow();
}

GlfwWindow::~GlfwWindow() { glfwTerminate(); }

void GlfwWindow::BuildGlfwWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  m_window =
      glfwCreateWindow((int)m_size.x, (int)m_size.y, m_title.c_str(), nullptr, nullptr);

  if (!m_window) {
    throw std::runtime_error("Can't setup monitor");
  }
}

void GlfwWindow::Start() {
    glfwMakeContextCurrent(m_window); }

void GlfwWindow::ClearBuffer() {  }

void GlfwWindow::SwapBuffers() { glfwSwapBuffers(m_window); }

void GlfwWindow::PollEvents() { glfwPollEvents(); }

bool GlfwWindow::IsShouldClose() const { return glfwWindowShouldClose(m_window); }
