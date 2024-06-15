//
// Created by FullHat on 21/05/2024.
//

#include "InputManager.hpp"

void InputManager::bindAction(uint64_t Key, std::function<void()> Action) {
  KeyBindings[Key] = Action;

  glfwSetKeyCallback(NativeType, &InputManager::keyCallback);
}
void InputManager::keyCallback(GLFWwindow *Window, int Key, int Scancode,
                               int Action, int Mods) {
  for (auto &KeyBinding : KeyBindings) {
    if (KeyBinding.first == Key) {
      KeyBinding.second();
    }
  }
}

void InputManager::bindMouseXY(std::function<void(double, double)> Action) {
  MouseXY = Action;

  glfwSetCursorPosCallback(NativeType, &InputManager::mouseXYCallback);
}

void InputManager::mouseXYCallback(GLFWwindow *Window, double X, double Y) {
  MouseXY(X, Y);
}
