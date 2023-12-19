#include "Engine.hpp"

#include "Core/DebugMessanger.hpp"
#include "Utils/Sync.hpp"

#include <iostream>

void Engine::RunMainCycle() {
    m_window->Start();
    Scene scene;
    MakeAssets();
    while (!m_window->IsShouldClose()) {
        m_window->PollEvents();
        Render(scene);
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

    // Make physical device
    m_gpu = std::make_unique<Gpu>(m_instance, *m_window);
    auto devices = m_gpu->GetAllAvailable();
    m_gpu->SetCurrent(devices[0]);

    // Make shader system
    GraphicsPipeline::GraphicsPipelineInBundle specification(m_gpu->GetLogicalDevice());
    Shader::compile_shader("/Users/full-hat/Documents/UnrealProjects/pie-engine/Shaders/Shaders/empty_f.frag", "./");
    Shader::compile_shader("/Users/full-hat/Documents/UnrealProjects/pie-engine/Shaders/Shaders/empty_v.vert", "./");
    specification.vertexFilepath = "./empty_v.spv";
    specification.fragmentFilepath = "./empty_f.spv";
    specification.swapchainExtent = m_gpu->GetSwapChainExtent();
    specification.swapchainImageFormat = m_gpu->GetSwapChainFormat();

    m_pipeline = std::make_unique<GraphicsPipeline>(specification);

    Gpu::framebufferInput frameBufferInput;
    frameBufferInput.device = m_gpu->GetLogicalDevice();
    frameBufferInput.renderpass = m_pipeline->GetRenderPass();
    frameBufferInput.swapchainExtent = m_gpu->GetSwapChainExtent();
    m_gpu->make_framebuffers(frameBufferInput);

    m_gpu->make_command_pool();
    m_gpu->make_main_command_buffer();
    m_gpu->make_frame_command_buffers();
}

Engine::~Engine() {
    m_gpu->DestroyCommandPool();
    m_pipeline.reset();
    m_mesh.reset();
    m_gpu.reset();

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

void Engine::Render(Scene _scene) {
    m_gpu->Render(*m_pipeline, _scene, *m_mesh);
}

void Engine::MakeAssets() {
    m_mesh = std::make_unique<MeshesManager>();

    std::vector<float> vertices = { {
                                            0.0f, -0.05f, 0.0f, 1.0f, 0.0f,
                                            0.05f, 0.05f, 0.0f, 1.0f, 0.0f,
                                            -0.05f, 0.05f, 0.0f, 1.0f, 0.0f
                                    } };
    Mesh::Types type = Mesh::Types::eTriangle;
    m_mesh->Consume(type, vertices);

    vertices = { {
                         -0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
                         -0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
                         0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
                         0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
                         0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
                         -0.05f,  0.05f, 1.0f, 0.0f, 0.0f
                 } };
    type = Mesh::Types::eSquare;
    m_mesh->Consume(type, vertices);

    m_mesh->Finalize(m_gpu->GetLogicalDevice(), m_gpu->GetDevice());
}
