#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "Core/Window/GlfwWindow.hpp"
#include "Core/Instance.hpp"
#include "Core/Gpu.hpp"
#include "Shaders/Pipeline.hpp"
#include "Project/Scene.hpp"
#include "Project/MeshesManager.hpp"
#include "Core/Renderer.hpp"

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
    void MakeAssets();

protected:
    std::shared_ptr<peVk::Instance> m_instance{nullptr};

    vk::DebugUtilsMessengerEXT m_debug_messenger{ nullptr };
    vk::DispatchLoaderDynamic m_dldi;

    std::unique_ptr<Renderer> m_renderer;

    std::shared_ptr<GlfwWindow> m_window;

    std::unique_ptr<Scene> m_scene;
};
