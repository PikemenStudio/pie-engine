//
// Created by FullHat on 21/05/2024.
//

#ifndef ENGINE_SRC_MODULES_INPUT_MANAGER_SRC_INPUTMANAGER_HPP
#define ENGINE_SRC_MODULES_INPUT_MANAGER_SRC_INPUTMANAGER_HPP

#include "GLFW/glfw3.h"
#include <functional>
#include <map>

class InputManager {
public:
  inline static GLFWwindow *NativeType;

  struct KeyBinding {
    std::function<void()> Action;
  };

  using KeyBindingsType = std::map<uint64_t, std::function<void()>>;

  static void bindAction(uint64_t Key, std::function<void()> Action);

  static void bindMouseXY(std::function<void(double, double)> Action);

protected:
  static void keyCallback(GLFWwindow *Window, int Key, int Scancode, int Action,
                          int Mods);

  static void mouseXYCallback(GLFWwindow *Window, double X, double Y);

protected:
  inline static KeyBindingsType KeyBindings;
  inline static std::function<void(double, double)> MouseXY;
};

#endif // ENGINE_SRC_MODULES_INPUT_MANAGER_SRC_INPUTMANAGER_HPP
