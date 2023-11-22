#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "Window/Window.hpp"

class Engine {
public:
    Engine();
    Engine(const glm::uvec2 _window_size, std::string window_title);

    void RunMainCycle();

protected:

protected:
    vk::Instance m_instance{nullptr};

    std::unique_ptr<Window> m_window;
};
