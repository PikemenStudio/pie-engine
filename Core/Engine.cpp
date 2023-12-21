#include "Engine.hpp"

#include "Core/DebugMessanger.hpp"
#include "Utils/Sync.hpp"

#include <iostream>

void Engine::RunMainCycle() {
    m_window->Start();
    m_scene = std::make_unique<Scene>();

    MakeAssets();
    m_renderer->SetSceneReference(m_scene.get());
    while (!m_window->IsShouldClose()) {
        m_window->PollEvents();
        Render();
        CalculateFrameRate();
    }
}

Engine::Engine() :
        Engine(glm::vec2(300, 200), "Engine") {
}

Engine::Engine(const glm::uvec2 _window_size, std::string _window_title) {
    try {
        // Make window
        m_window = std::make_unique<GlfwWindow>(_window_size, std::move(_window_title));
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "Can't setup window" << std::endl;
        throw;
    }

    m_instance = std::make_shared<peVk::Instance>("engine");
    m_dldi = vk::DispatchLoaderDynamic(*m_instance->ToVkInstancePtr(), vkGetInstanceProcAddr);
    m_debug_messenger = make_debug_messenger(m_instance->ToVkInstancePtr(), m_dldi);

    // Create renderer
    Renderer::RendererCreateInfo inf {
        *m_instance,
        m_window
    };
    m_renderer = std::make_unique<Renderer>(inf);

    // Init renderer fields
    m_renderer->InitGpu();
    m_renderer->InitPipeline();
    m_renderer->InitBuffers();
}

Engine::~Engine() {
    m_renderer.reset();

    m_instance->ToVkInstancePtr()->destroyDebugUtilsMessengerEXT(m_debug_messenger, nullptr, m_dldi);
    m_instance->ToVkInstancePtr()->destroy();
}

void Engine::CalculateFrameRate() {
    currentTime = glfwGetTime();
    double delta = currentTime - lastTime;

    if (delta >= 1) {
        int framerate{ std::max(1, int(numFrames / delta)) };
        std::stringstream title;
        title << "Running at " << framerate << " fps.";
        glfwSetWindowTitle(&m_window->ToWindow(), title.str().c_str());
        lastTime = currentTime;
        numFrames = -1;
        frameTime = float(1000.0 / framerate);
    }

    ++numFrames;
}

void Engine::Render() {
    m_renderer->Render();
}

void Engine::MakeAssets() {
    std::vector<float> vertices = { {
                                            0.0f, -0.05f, 0.0f, 1.0f, 0.0f,
                                            0.05f, 0.05f, 0.0f, 1.0f, 0.0f,
                                            -0.05f, 0.05f, 0.0f, 1.0f, 0.0f
                                    } };
    std::string type = "triangle";
    m_scene->GetMeshes().Consume(type, vertices);

    vertices = { {
                         -0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
                         -0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
                         0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
                         0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
                         0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
                         -0.05f,  0.05f, 1.0f, 0.0f, 0.0f
                 } };
    type = "square";
    m_scene->GetMeshes().Consume(type, vertices);
}
