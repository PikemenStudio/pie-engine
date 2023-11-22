#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vulkan/vulkan.hpp>

#include "GLFW/glfw3.h"

template <typename T>
concept StringLike =
            std::is_same_v<std::remove_cvref<T>, std::string> ||
            std::is_convertible_v<T, const char*>;

class Window {
public:
    template <StringLike Str>
    Window(const glm::uvec2 &_size, Str &&_title)
            : m_size(_size), m_title(std::forward<Str &&>(_title)) {
        BuildGlfwWindow();
    }

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

    [[nodiscard]]
    bool IsShouldClose() const;

protected:
    glm::uvec2 m_size;
    bool m_is_fullscreen{false};
    std::string m_title;

    GLFWwindow* m_window{nullptr};
};
