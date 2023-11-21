#include "Engine.hpp"

#include "Core/Window/Window.hpp"

#include <iostream>

void Engine::RunMainCycle() {
    m_window->Start();
    while (!m_window->IsShouldClose()) {
        m_window->ClearBuffer();
        m_window->SwapBuffers();
        m_window->PollEvents();
    }
}

Engine::Engine() {
    BuildInstance();
}

void Engine::BuildInstance() {
    try {
        m_window = std::make_unique<Window>(glm::uvec2(300, 200), "Engine");
        m_window->BuildGlfwWindow();
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "Can't setup window" << std::endl;
    }
}
