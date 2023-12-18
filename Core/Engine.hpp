#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "Core/Window/GlfwWindow.hpp"
#include "Core/Instance.hpp"
#include "Core/Gpu.hpp"
#include "Shaders/Pipeline.hpp"

class Engine {
public:
    Engine();
    Engine(const glm::uvec2 _window_size, std::string window_title);

    ~Engine();

    void RunMainCycle();

    void Render();

    double lastTime, currentTime;
    int numFrames;
    float frameTime;
    void CalculateFrameRate();

protected:

protected:
    std::shared_ptr<peVk::Instance> m_instance{nullptr};

    vk::DebugUtilsMessengerEXT m_debug_messenger{ nullptr };
    vk::DispatchLoaderDynamic m_dldi;

    // Current GPU
    std::unique_ptr<Gpu> m_gpu;

    std::unique_ptr<GlfwWindow> m_window;

    std::unique_ptr<GraphicsPipeline> m_pipeline;
};
