cmake_minimum_required(VERSION 3.5.0)

set(LOGURU_WITH_STREAMS TRUE)

# Set global flags

if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
endif (UNIX AND NOT APPLE)

set(SFML_WARNINGS_AS_ERRORS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_CXX_STANDARD 20 REQUIRED)

add_compile_options("-std=c++20")

option(TESTS "Build tests (tests will compile modules)" OFF)

project(Engine VERSION 0.0.1 LANGUAGES C CXX)

# compile tests
add_subdirectory(src/modules/windows/src)
add_subdirectory(src/modules/graphics/src)
add_subdirectory(src/modules/sound/src)
add_subdirectory(src/modules/shader_loader/src)
add_subdirectory(src/modules/renderable_scene/src)
add_subdirectory(src/modules/scene_manager/src)
add_subdirectory(src/modules/objects)
add_subdirectory(src/modules/input_manager)
#add_subdirectory(src/modules/imgui_docking)
set(ADAPTERS ${ADAPTERS}
        src/modules/windows/facades/facade.hpp
        src/modules/graphics/facades/facade.hpp
        src/modules/sound/facade/facade.hpp
        src/modules/shader_loader/facades/facade.hpp
        src/modules/renderable_scene/facades/facade.hpp
        src/modules/scene_manager/facades/facade.hpp
        src/modules/input_manager/InputManager.hpp
)

add_subdirectory(tests)
add_subdirectory(game)

add_executable(main src/main.cpp ${ADAPTERS})

# link module
target_link_libraries(main PRIVATE windows)
include_directories(src/modules/windows/facades)

target_link_libraries(main PRIVATE graphics)
include_directories(src/modules/graphics/facades)

target_link_libraries(main PRIVATE sound)
include_directories(src/modules/sound/facade)

target_link_libraries(main PRIVATE shader_loader)
include_directories(src/modules/shader_loader/facades)

target_link_libraries(main PRIVATE renderable_scene)
include_directories(src/modules/renderable_scene/facades)

target_link_libraries(main PRIVATE scene_manager)
include_directories(src/modules/scene_manager/facades)

target_link_libraries(main PRIVATE objects)
include_directories(src/modules/objects)

target_link_libraries(main PRIVATE input_manager)
include_directories(src/modules/input_manager/facades)

find_package(loguru CONFIG REQUIRED)
target_link_libraries(main PRIVATE loguru)

# define path constants
target_compile_definitions(main PRIVATE VULKAN_SDK_PATH="$ENV{VULKAN_SDK}" PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
