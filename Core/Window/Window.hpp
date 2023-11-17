#include <glm/glm.hpp>
#include <iostream>
#include <vulkan/vulkan.hpp>

#include "GLFW/glfw3.h"

class Window {
 public:
  Window(const glm::uvec2 &_size, const std::string &_title);
  Window();

  ~Window();

protected:
  /**
   * @brief Create window using GLFW library
   * @throw std::runtime "Can't setup monitor" if creation failed
  */
  void BuildGlfwWindow();

public:
  void Start();

  void ClearBuffer();

  void SwapBuffers();

  void PollEvents();

  bool IsShouldClose() const;

 protected:
  glm::uvec2 m_size;
  bool m_is_fullscreen{false};
  std::string m_title;

  GLFWwindow* m_window{nullptr};
};
