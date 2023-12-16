#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vulkan/vulkan.hpp>

#include "GLFW/glfw3.h"

class GlfwWindow {
public:
    GlfwWindow(const glm::uvec2 &_size, std::string _title);

    GlfwWindow();

    ~GlfwWindow();

    glm::vec2 GetSize() { return m_size; }

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

    [[nodiscard]]
    bool IsShouldClose() const;

    [[nodiscard]] GLFWwindow& ToWindow() const { return *m_window; }

protected:
    glm::uvec2 m_size;
    bool m_is_fullscreen{false};
    std::string m_title;

    GLFWwindow* m_window{nullptr};
};
