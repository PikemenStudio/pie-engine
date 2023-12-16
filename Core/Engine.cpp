#include "Engine.hpp"

#include "Core/DebugMessanger.hpp"

#include <iostream>

void Engine::RunMainCycle() {
    m_window->Start();
    while (!m_window->IsShouldClose()) {
        m_window->ClearBuffer();
        m_window->SwapBuffers();
        m_window->PollEvents();
    }
}

Engine::Engine() :
        Engine(glm::vec2(300, 200), "Engine") {
}

Engine::Engine(const glm::uvec2 _window_size, std::string _window_title) {
    try {
        // Make window
        m_window = std::make_unique<Window>(_window_size, std::move(_window_title));
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
}

Engine::~Engine() {
    m_gpu.reset();

    m_instance->ToVkInstancePtr()->destroyDebugUtilsMessengerEXT(m_debug_messenger, nullptr, m_dldi);
    m_instance->ToVkInstancePtr()->destroy();
}
