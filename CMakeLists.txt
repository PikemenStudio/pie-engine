cmake_minimum_required(VERSION 3.5.0)

# Set global flags
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_CXX_STANDARD 20)

project(Engine VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src/modules/vk_core)
add_subdirectory(src/modules/windows)

add_executable(main src/main.cpp)

# link module
target_link_libraries(main PRIVATE vk_core)
include_directories(src/modules/vk_core/src)

target_link_libraries(main PRIVATE windows)
include_directories(src/modules/windows/src)


#add_subdirectory(tests)

# define path constants
target_compile_definitions(main PRIVATE VULKAN_SDK_PATH="$ENV{VULKAN_SDK}" PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
