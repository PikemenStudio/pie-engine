#include "Engine.hpp"

#include "Core/Window/Window.hpp"
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

Engine::Engine(const glm::uvec2 _window_size, std::string window_title) {
    try {
        // Make window
        m_window = std::make_unique<Window>(std::move(_window_size), "Engine");
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "Can't setup window" << std::endl;
        throw;
    }

    m_instance = std::make_unique<peVk::Instance>("engine");
    m_dldi = vk::DispatchLoaderDynamic(*m_instance->ToVkInstancePtr(), vkGetInstanceProcAddr);
    m_debug_messenger = make_debug_messenger(m_instance->ToVkInstancePtr(), m_dldi);
}

Engine::~Engine() {
    m_instance->ToVkInstancePtr()->destroyDebugUtilsMessengerEXT(m_debug_messenger, nullptr, m_dldi);
    m_instance->ToVkInstancePtr()->destroy();
}
