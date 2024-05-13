//
// Created by FullHat on 01/04/2024.
//

#ifndef ENGINE_SRC_MODULES_WINDOWS_FACADES_FACADE_HPP
#define ENGINE_SRC_MODULES_WINDOWS_FACADES_FACADE_HPP

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Define all structures to be used in the facades
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct WindowFacadeStructs {
  struct WindowProps {
    glm::vec2 Size{200, 200};
    std::string Title;

    enum WindowMode { WINDOWED, BORDERLESS, FULLSCREEN } Mode = WINDOWED;

    // Used only if Mode == WINDOWED
    bool IsResizable{true};
  };
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Concept to get errors earlier if the Impl is not valid
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename T>
concept WindowApiImpl = requires(T Obj) {
  std::is_constructible_v<T, WindowFacadeStructs::WindowProps>;
  { Obj.getNativeType() } -> std::same_as<void *>;
  { Obj.getRequiredExtensions() } -> std::same_as<std::vector<const char *>>;
  { Obj.getSize() } -> std::same_as<std::pair<uint32_t, uint32_t>>;
  { Obj.prepare() } -> std::same_as<void>;
  { Obj.shouldClose() } -> std::same_as<bool>;
  { Obj.update() } -> std::same_as<void>;
  { Obj.setTitle(std::declval<const std::string &>()) } -> std::same_as<void>;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blueprint for the Window API Implementation, add functions here
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define WINDOW_API_IMPL(name)                                                  \
  class WindowApiFacade##name##Impl {                                          \
  public:                                                                      \
    WindowApiFacade##name##Impl(const WindowApiFacade##name##Impl &) = delete; \
    WindowApiFacade##name##Impl(WindowApiFacade##name##Impl &&Obj) {           \
      this->Data = nullptr;                                                    \
      std::swap(this->Data, Obj.Data);                                         \
    }                                                                          \
    WindowApiFacade##name##Impl(WindowFacadeStructs::WindowProps &&);          \
    ~WindowApiFacade##name##Impl();                                            \
                                                                               \
    void *getNativeType() const;                                               \
    std::pair<uint32_t, uint32_t> getSize() const;                             \
                                                                               \
    std::vector<const char *> getRequiredExtensions() const;                   \
                                                                               \
    void prepare();                                                            \
    bool shouldClose();                                                        \
    void update();                                                             \
    void swapBuffers();                                                        \
    void setTitle(const std::string &Title);                                   \
                                                                               \
  protected:                                                                   \
    void *Data = nullptr;                                                      \
  };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Ready to use implementations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace window_api_impls {
WINDOW_API_IMPL(Glfw)
} // namespace window_api_impls

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Facade for the Window API
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <WindowApiImpl Impl = window_api_impls::WindowApiFacadeGlfwImpl>
class WindowApiFacade {
public:
  WindowApiFacade(WindowFacadeStructs::WindowProps &&Props)
      : ImplInstance(std::move(Props)) {}

public:
  [[maybe_unused]] Impl ImplInstance;
};

#endif // ENGINE_SRC_MODULES_WINDOWS_FACADES_FACADE_HPP
