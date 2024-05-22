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

protected:
  static void keyCallback(GLFWwindow *Window, int Key, int Scancode, int Action,
                          int Mods);

protected:
  inline static KeyBindingsType KeyBindings;
};

#endif // ENGINE_SRC_MODULES_INPUT_MANAGER_SRC_INPUTMANAGER_HPP
