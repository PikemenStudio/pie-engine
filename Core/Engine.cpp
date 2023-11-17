#include "Engine.hpp"

#include "Core/Window/Window.hpp"

#include <iostream>

void Engine::RunMainCycle() { Window window({100, 100}, "Pie Engine");
  window.Start();
  while (!window.IsShouldClose()) {
    window.ClearBuffer();
    window.SwapBuffers();
    window.PollEvents();
  }
}
