#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "Window/Window.hpp"

class Engine {
public:
    Engine();

    void RunMainCycle();

protected:
    void BuildInstance();

protected:
    vk::Instance m_instance{nullptr};

    std::unique_ptr<Window> m_window;
};
