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

Engine::Engine() :
        Engine(glm::vec2(300, 200), "Engine") {
}

Engine::Engine(const glm::uvec2 _window_size, std::string window_title) {
    try {
        // Make window
        m_window = std::make_unique<Window>(glm::uvec2(300, 200), "Engine");
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "Can't setup window" << std::endl;
        throw;
    }
}
