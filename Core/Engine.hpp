#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "Window/Window.hpp"
#include "Core/Instance.hpp"
#include "Core/gpu.hpp"

class Engine {
public:
    Engine();
    Engine(const glm::uvec2 _window_size, std::string window_title);

    ~Engine();

    void RunMainCycle();

protected:

protected:
    std::shared_ptr<peVk::Instance> m_instance{nullptr};

    vk::DebugUtilsMessengerEXT m_debug_messenger{ nullptr };
    vk::DispatchLoaderDynamic m_dldi;

    // Current GPU
    std::unique_ptr<Gpu> m_gpu;

    std::unique_ptr<Window> m_window;
};
